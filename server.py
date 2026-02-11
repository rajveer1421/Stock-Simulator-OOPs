import os, subprocess,json,random
from flask import Flask, render_template, request, redirect, session, flash

app = Flask(__name__)
app.secret_key = "rajveer_stock_secret"

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
CPP_EXEC = os.path.join(BASE_DIR, "main.exe")
if not os.path.exists(CPP_EXEC):
    alt = os.path.join(BASE_DIR, "main")
    if os.path.exists(alt):
        CPP_EXEC = alt

def run_cpp(input_script, timeout=10):
    try:
        p = subprocess.Popen([CPP_EXEC], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT, text=True, cwd=BASE_DIR)
        out,_ = p.communicate(input=input_script, timeout=timeout)
        return out
    except:
        return "[ERROR] C++ program timed out (likely waiting for more input)"

# ------------------ HOME ------------------
@app.route("/")
def home():
    return render_template("index.html")

# ------------------ TRADER LOGIN ------------------
@app.route("/trader_login", methods=["GET", "POST"])
def trader_login():
    if request.method == "POST":
        name = request.form["name"].strip()
        password = request.form["password"].strip()

        trader_file = f"Traders/{name}.json"

        # Check user exists
        if not os.path.exists(trader_file):
            flash("❌ Trader not found, please register first!", "error")
            return redirect("/trader_login")

        # Load trader data
        with open(trader_file, "r") as f:
            data = json.load(f)

        # Compare passwords
        if data["password"] != password:
            flash("❌ Incorrect password!", "error")
            return redirect("/trader_login")

        # If password correct → allow login
        session["trader"] = name
        session["trader_pass"] = password

        return redirect("/trader_dashboard")

    return render_template("trader_login.html")


@app.route("/trader_dashboard")
def trader_dashboard():
    if "trader" not in session:
        return redirect("/trader_login")
    return render_template("trader_dashboard.html", trader=session["trader"])

@app.route("/view_market",methods=["GET"])
def view_market():
    if "trader" not in session:
        return redirect("/trader_login")

    stocks = []
    stocks_folder = os.path.join(BASE_DIR, "Stocks")

    for filename in os.listdir(stocks_folder):
        if filename.endswith(".json"):
            path = os.path.join(stocks_folder, filename)
            with open(path, "r") as f:
                data = json.load(f)
                stocks.append({
                    "company": data.get("company_name"),
                    "price": data.get("current_price"),
                    "available": data.get("stock_available"),
                    "sold": data.get("stock_sold")
                })

    return render_template("market.html", stocks=stocks)
# ------------------ VIEW INVESTMENTS ------------------
@app.route("/view_investments",methods=["GET"])
def transactions():
    if "trader" not in session:
        return redirect("/trader_login")

    trader = session["trader"]
    file_path = os.path.join("Traders", f"{trader}.json")

    if not os.path.exists(file_path):
        flash("Trader file missing.")
        return redirect("/trader_dashboard")

    with open(file_path, "r") as f:
        data = json.load(f)

    transactions = data.get("transactions", [])

    return render_template("investment.html", transactions=transactions)
# ------------------ VIEW DETAILS ------------------
@app.route("/view_details",methods=["GET"])
def view_details():
    if "trader" not in session:
        return redirect("/trader_login")

    trader = session["trader"]
    file_path = os.path.join("Traders", f"{trader}.json")

    if not os.path.exists(file_path):
        flash("Trader data not found.")
        return redirect("/trader_dashboard")

    with open(file_path, "r") as f:
        data = json.load(f)

    name = data.get("name", "")
    init_balance = data.get("initial_balance", 0)
    balance = data.get("balance", 0)
    holdings = len(data.get("portfolio", []))

    return render_template("details.html",
                           name=name,
                           init_balance=init_balance,
                           balance=balance,
                           holdings=holdings)
# ------------------ DEPOSIT / WITHDRAW ------------------
@app.route("/deposit_withdraw", methods=["GET","POST"])
def deposit_withdraw():
    if "trader" not in session:
        return redirect("/trader_login")

    trader = session["trader"]
    file_path = os.path.join("Traders", f"{trader}.json")

    with open(file_path, "r") as f:
        data = json.load(f)

    current_balance = data.get("balance", 0)

    if request.method == "POST":
        amount = float(request.form["amount"])
        action = request.form["action"]

        if amount <= 0:
            flash("Amount must be greater than 0", "error")
            return redirect("/deposit_withdraw")

        if action == "deposit":
            data["balance"] = current_balance + amount
            flash(f"Deposited ₹{amount} successfully!", "success")

        elif action == "withdraw":
            if amount > current_balance:
                flash("Insufficient balance!", "error")
                return redirect("/deposit_withdraw")
            data["balance"] = current_balance - amount
            flash(f"Withdrawn ₹{amount} successfully!", "success")

        with open(file_path, "w") as f:
            json.dump(data, f, indent=4)

        return redirect("/deposit_withdraw")

    return render_template("deposit_withdraw.html", balance=current_balance)
 # ------------------ UPDATE PASSWORD ------------------
@app.route("/update_password", methods=["GET", "POST"])
def update_password():
    if "trader" not in session:
        return redirect("/trader_login")

    trader = session["trader"]
    file_path = os.path.join("Traders", f"{trader}.json")

    with open(file_path, "r") as f:
        data = json.load(f)

    if request.method == "POST":
        old_pass = request.form["old_password"]
        new_pass = request.form["new_password"]
        confirm_pass = request.form["confirm_password"]

        # ✅ Check old password
        if old_pass != data.get("password"):
            flash("❌ Old password is incorrect!", "error")
            return redirect("/update_password")

        # ✅ New pass match check
        if new_pass != confirm_pass:
            flash("❌ New passwords do not match!", "error")
            return redirect("/update_password")

        # ✅ Update JSON
        data["password"] = new_pass
        with open(file_path, "w") as f:
            json.dump(data, f, indent=4)

        flash("✅ Password updated successfully!", "success")
        return redirect("/trader_dashboard")

    return render_template("update_password.html")
# ------------------ SIP CALCULATOR ------------------
@app.route("/sip_calculator", methods=["GET", "POST"])
def sip_calculator():
    if "trader" not in session:
        return redirect("/trader_login")

    result = None

    if request.method == "POST":
        company = request.form["company"]
        monthly = float(request.form["monthly"])
        years = int(request.form["years"])
        
        company_file = os.path.join("companies", f"{company}.json")

        if not os.path.exists(company_file):
            flash("Company not found!", "error")
            return redirect("/sip_calculator")

        with open(company_file, "r") as f:
            data = json.load(f)

        sip_rate = float(data.get("sipRate", 0))
        months = years * 12
        r = sip_rate / 100 / 12

        # SIP formula = P * [((1+r)^n - 1)/r] * (1+r)
        final_value = monthly * (((1+r)**months - 1) / r) * (1+r)
        invested = monthly * months
        profit = final_value - invested

        result = {
            "company": company,
            "monthly": monthly,
            "years": years,
            "invested": round(invested, 2),
            "profit": round(profit, 2),
            "final": round(final_value, 2),
            "rate": sip_rate
        }

    return render_template("sip_calculator.html", result=result)
# ------------------ UPDATE STOCK FILE ------------------
def update_stock_file(company, qty_change, is_buy):
    """
    Updates Stocks/<company>.json safely after buy or sell.

    - If is_buy = True → stock_available decreases, stock_sold increases, price increases.
    - If is_buy = False → stock_available increases, stock_sold decreases, price decreases.
    """
    stock_file = os.path.join("Stocks", f"{company}.json")

    if not os.path.exists(stock_file):
        return False, f"❌ Stock file not found for {company}"

    with open(stock_file, "r") as f:
        stock_data = json.load(f)

    available = stock_data.get("stock_available", 0)
    sold = stock_data.get("stock_sold", 0)
    current_price = float(stock_data.get("current_price", 0.0))

    # ✅ PRICE CHANGE LOGIC (same as C++ Stock.cpp)
    if is_buy:
        if qty_change > available:
            return False, f"⚠️ Not enough stock of {company} in market"

        available -= qty_change
        sold += qty_change

        # Increase price 2% per unit
        current_price += current_price * 0.02 * qty_change

    else:
        available += qty_change
        sold = max(0, sold - qty_change)

        # Decrease price 2% per unit
        current_price -= current_price * 0.02 * qty_change

    # ✅ Keep price realistic
    if current_price < 1.0:
        current_price = 1.0
    current_price = round(current_price, 2)

    # ✅ Save back
    stock_data.update({
        "stock_available": available,
        "stock_sold": sold,
        "current_price": current_price
    })

    with open(stock_file, "w") as f:
        json.dump(stock_data, f, indent=4)

    return True, f"Market updated successfully for {company}"


# ------------------ BUY STOCK ------------------
@app.route("/buy_stock", methods=["GET", "POST"])
@app.route("/buy_stock", methods=["GET", "POST"])
def buy_stock():
    if "trader" not in session:
        return redirect("/trader_login")

    trader_name = session["trader"]
    trader_file = os.path.join("Traders", f"{trader_name}.json")

    if not os.path.exists(trader_file):
        flash("Trader file not found!", "error")
        return redirect("/trader_dashboard")

    # Load trader data
    with open(trader_file, "r") as f:
        trader_data = json.load(f)

    balance = trader_data.get("balance", 0)
    portfolio = {p["company_name"]: p["quantity"] for p in trader_data.get("portfolio", [])}

    if request.method == "POST":
        company = request.form["company"].strip()
        qty = int(request.form["quantity"])

        if qty <= 0:
            flash("❌ Invalid quantity!", "error")
            return redirect("/buy_stock")

        stock_file = os.path.join("Stocks", f"{company}.json")
        if not os.path.exists(stock_file):
            flash("❌ Company not found!", "error")
            return redirect("/buy_stock")

        # Load stock data
        with open(stock_file, "r") as f:
            stock_data = json.load(f)

        price = float(stock_data.get("current_price", 0))
        available = int(stock_data.get("stock_available", 0))
        total_cost = qty * price

        if total_cost > balance:
            flash("💰 Insufficient balance!", "error")
            return redirect("/buy_stock")

        if qty > available:
            flash("⚠️ Not enough stock available in market!", "error")
            return redirect("/buy_stock")

        # ✅ Update trader balance
        balance -= total_cost
        trader_data["balance"] = balance

        # ✅ Update portfolio
        portfolio[company] = portfolio.get(company, 0) + qty
        trader_data["portfolio"] = [{"company_name": k, "quantity": v} for k, v in portfolio.items()]

        # ✅ Record transaction
        if "transactions" not in trader_data:
            trader_data["transactions"] = []
        trader_data["transactions"].append({
            "type": "buy",
            "stock_id": company,
            "quantity": qty,
            "price": price
        })

        # ✅ Save updated trader data
        with open(trader_file, "w") as f:
            json.dump(trader_data, f, indent=4)

        # ✅ Update stock file in market
        success, msg = update_stock_file(company, qty, is_buy=True)
        if not success:
            flash(msg, "error")
            return redirect("/buy_stock")

        flash(f"✅ Bought {qty} shares of {company} at ₹{price:.2f} (Total ₹{total_cost:.2f})", "success")
        return redirect("/buy_stock")

    return render_template("buy_stock.html", balance=balance)

# ------------------ SELL STOCK ------------------
@app.route("/sell_stock", methods=["GET", "POST"])
def sell_stock():
    if "trader" not in session:
        return redirect("/trader_login")

    trader_name = session["trader"]
    trader_file = os.path.join("Traders", f"{trader_name}.json")

    if not os.path.exists(trader_file):
        flash("Trader file not found!", "error")
        return redirect("/trader_dashboard")

    # Load trader data
    with open(trader_file, "r") as f:
        trader_data = json.load(f)

    balance = trader_data.get("balance", 0)
    portfolio = {p["company_name"]: p["quantity"] for p in trader_data.get("portfolio", [])}

    if request.method == "POST":
        company = request.form["company"].strip()
        qty = int(request.form["quantity"])

        if qty <= 0:
            flash("❌ Invalid quantity!", "error")
            return redirect("/sell_stock")

        if company not in portfolio or portfolio[company] < qty:
            flash("⚠️ Not enough stock in your holdings!", "error")
            return redirect("/sell_stock")

        stock_file = os.path.join("Stocks", f"{company}.json")
        if not os.path.exists(stock_file):
            flash("❌ Company not found!", "error")
            return redirect("/sell_stock")

        # Load stock data
        with open(stock_file, "r") as f:
            stock_data = json.load(f)

        price = float(stock_data.get("current_price", 0))
        total_value = qty * price

        # ✅ Update trader balance
        balance += total_value
        trader_data["balance"] = balance

        
        portfolio[company] -= qty
        if portfolio[company] <= 0:
            del portfolio[company]
        trader_data["portfolio"] = [{"company_name": k, "quantity": v} for k, v in portfolio.items()]

        
        if "transactions" not in trader_data:
            trader_data["transactions"] = []
        trader_data["transactions"].append({
            "type": "sell",
            "stock_id": company,
            "quantity": qty,
            "price": price
        })

        
        with open(trader_file, "w") as f:
            json.dump(trader_data, f, indent=4)

        
        success, msg = update_stock_file(company, qty, is_buy=False)
        if not success:
            flash(msg, "error")
            return redirect("/sell_stock")

        flash(f"✅ Sold {qty} shares of {company} for ₹{total_value:.2f}", "success")
        return redirect("/sell_stock")

    return render_template("sell_stock.html", balance=balance, portfolio=portfolio)


# ------------------ LEADERBOARD MENU ------------------
@app.route("/leaderboard")
def leaderboard_menu():
    """
    1️⃣ Runs C++ to auto-generate Leaderboard.json
    2️⃣ Reads Leaderboard.json
    3️⃣ Displays it nicely on leaderboard_table.html
    """

    
    script = "5\n1\n1\n2\n6\n"
    output = run_cpp(script)

    # 🟢 Step 2: Locate Leaderboard.json file
    leaderboard_file = os.path.join(BASE_DIR, "Leaderboard.json")

    if not os.path.exists(leaderboard_file):
        flash("❌ Leaderboard not found! C++ may not have generated it.", "error")
        print(output)
        return redirect("/")

    
    with open(leaderboard_file, "r") as f:
        try:
            data = json.load(f)
        except json.JSONDecodeError:
            flash("⚠️ Leaderboard file is corrupted or empty.", "error")
            return redirect("/")

    
    rows = sorted(data, key=lambda x: x.get("rank", 0))

    
    return render_template(
        "leaderboard_table.html",
        title="🏆 Live Leaderboard",
        rows=rows
    )


# ------------------ LOGOUT ------------------
@app.route("/logout")
def logout():
    session.clear()
    return redirect("/")

# ------------------ COMPANY LOGIN  ------------------
@app.route("/company_login", methods=["GET", "POST"])
def company_login():
    if request.method == "POST":
        name = request.form["name"]
        password = request.form["password"]

        company_file = os.path.join("companies", f"{name}.json")
        if not os.path.exists(company_file):
            flash("❌ Company not found!", "error")
            return redirect("/company_login")

        with open(company_file, "r") as f:
            data = json.load(f)

        if data.get("password") != password:
            flash("❌ Incorrect password!", "error")
            return redirect("/company_login")

        session["company"] = name
        flash("✅ Company logged in successfully!", "success")
        return redirect("/company_dashboard")

    return render_template("company_login.html")
# ------------------ COMPANY DASHBOARD  ------------------
@app.route("/company_dashboard")
def company_dashboard():
    if "company" not in session:
        return redirect("/company_login")

    return render_template("company_menu.html", company=session["company"])
# ------------------ COMPANY DETAILS  ------------------
@app.route("/company_details")
def company_details():
    if "company" not in session:
        return redirect("/company_login")

    cname = session["company"]
    company_file = os.path.join("companies", f"{cname}.json")
    stock_file = os.path.join("Stocks", f"{cname}.json")

    with open(company_file, "r") as f:
        data = json.load(f)

    stock_available = 0
    if os.path.exists(stock_file):
        with open(stock_file, "r") as sf:
            sdata = json.load(sf)
            stock_available = sdata.get("stock_available", 0)

    return render_template(
        "company_details.html",
        data=data,
        stock_available=stock_available
    )

# ------------------ RAISE STOCK  ------------------
@app.route("/raise_stock", methods=["GET", "POST"])
def raise_stock():
    if "company" not in session:
        return redirect("/company_login")

    cname = session["company"]
    stock_file = os.path.join("Stocks", f"{cname}.json")

    if not os.path.exists(stock_file):
        flash("❌ Stock file missing for this company!", "error")
        return redirect("/company_dashboard")

    with open(stock_file, "r") as f:
        data = json.load(f)

    if request.method == "POST":
        qty = int(request.form["quantity"])
        data["stock_available"] += qty

        with open(stock_file, "w") as f:
            json.dump(data, f, indent=4)

        flash(f"✅ Added {qty} new stocks!", "success")
        return redirect("/raise_stock")

    return render_template("raise_stock.html", company=cname)
# ------------------ CHANGE SIP RATE  ------------------
@app.route("/change_sip", methods=["GET", "POST"])
def change_sip():
    if "company" not in session:
        return redirect("/company_login")

    cname = session["company"]
    file_path = os.path.join("companies", f"{cname}.json")

    with open(file_path, "r") as f:
        data = json.load(f)

    if request.method == "POST":
        new_rate = float(request.form["new_rate"])
        data["sipRate"] = new_rate

        with open(file_path, "w") as f:
            json.dump(data, f, indent=4)

        flash("✅ SIP Rate Updated!", "success")
        return redirect("/change_sip")

    return render_template("change_sip.html", rate=data["sipRate"])
# ------------------ UPDATE VALUATION  ------------------
@app.route("/update_valuation", methods=["GET", "POST"])
def update_valuation():
    if "company" not in session:
        return redirect("/company_login")

    company_name = session["company"]
    company_file = os.path.join("companies", f"{company_name}.json")


    with open(company_file, "r") as f:
        data = json.load(f)

    if request.method == "POST":
        new_val = float(request.form["new_valuation"])
        data["valuation"] = new_val


        with open(company_file, "w") as f:
            json.dump(data, f, indent=4)

        flash("✅ Valuation updated successfully!", "success")
        return redirect("/company_dashboard")

    return render_template("update_valuation.html", data=data)
# ------------------ TRADER REGISTER ------------------

@app.route("/trader_register", methods=["GET", "POST"])
def trader_register():
    if request.method == "POST":
        name = request.form["name"].strip()
        password = request.form["password"].strip()
        balance_raw = request.form["balance"].strip()

        # 🌟 Validate name
        if not name:
            flash("❌ Trader name cannot be empty!", "error")
            return redirect("/trader_register")

        # 🌟 Validate balance
        try:
            balance = float(balance_raw)
        except:
            flash("❌ Balance must be a valid number!", "error")
            return redirect("/trader_register")

        if balance <= 0:
            flash("❌ Initial balance must be greater than 0!", "error")
            return redirect("/trader_register")

        # 🌟 Password validation
        def is_valid(pw):
            if len(pw) <= 8:
                return False
            has_upper = any(c.isupper() for c in pw)
            has_digit = any(c.isdigit() for c in pw)
            has_symbol = any(not c.isalnum() for c in pw)
            return has_upper and has_digit and has_symbol

        if not is_valid(password):
            flash("❌ Password must be > 8 characters, contain a capital letter, a digit, and a symbol!", "error")
            return redirect("/trader_register")

        # 🌟 Check if trader exists
        trader_file = os.path.join("Traders", f"{name}.json")
        if os.path.exists(trader_file):
            flash("❌ Trader already exists!", "error")
            return redirect("/trader_register")

        os.makedirs("Traders", exist_ok=True)

        data = {
            "name": name,
            "password": password,
            "initial_balance": balance,
            "balance": balance,
            "portfolio": [],
            "transactions": []
        }

        with open(trader_file, "w") as f:
            json.dump(data, f, indent=4)

        flash("✅ Trader account created successfully!", "success")
        return redirect("/trader_login")

    return render_template("new_trader_register.html")

# ------------------ COMPANY REGISTER ------------------
@app.route("/company_register", methods=["GET", "POST"])
def company_register():
    if request.method == "POST":
        name = request.form["name"].strip()
        password = request.form["password"].strip()
        valuation_raw = request.form.get("valuation", "").strip()
        sip_rate_raw = request.form.get("sip_rate", "").strip()

        # -------- BASIC VALIDATION ----------
        if not name:
            flash("❌ Company name cannot be empty!", "error")
            return redirect("/company_register")

        # Password validation (same rules as trader)
        def is_valid(pw: str) -> bool:
            if len(pw) <= 8:
                return False
            has_upper = any(c.isupper() for c in pw)
            has_digit = any(c.isdigit() for c in pw)
            has_symbol = any(not c.isalnum() for c in pw)
            return has_upper and has_digit and has_symbol

        if not is_valid(password):
            flash("❌ Password must be > 8 characters, contain a capital letter, a digit, and a symbol!", "error")
            return redirect("/company_register")

        # Validate valuation + SIP
        try:
            valuation = float(valuation_raw)
            sip_rate = float(sip_rate_raw)
        except ValueError:
            flash("❌ Valuation and SIP rate must be valid numbers!", "error")
            return redirect("/company_register")

        if valuation <= 0:
            flash("❌ Valuation must be a positive number!", "error")
            return redirect("/company_register")

        if sip_rate < 0:
            flash("❌ SIP rate cannot be negative!", "error")
            return redirect("/company_register")
        # --------------------------------------

        # File paths
        company_file = os.path.join("companies", f"{name}.json")
        stock_file = os.path.join("Stocks", f"{name}.json")

        if os.path.exists(company_file):
            flash("❌ Company already exists!", "error")
            return redirect("/company_register")

        os.makedirs("companies", exist_ok=True)
        os.makedirs("Stocks", exist_ok=True)

        # Create company file
        company_data = {
            "name": name,
            "password": password,
            "valuation": valuation,
            "sipRate": sip_rate
        }

        with open(company_file, "w") as f:
            json.dump(company_data, f, indent=4)

        # Create stock file
        stock_data = {
            "company_name": name,
            "current_price": 100.0,
            "stock_available": 0,
            "stock_sold": 0
        }

        with open(stock_file, "w") as f:
            json.dump(stock_data, f, indent=4)

        flash("✅ Company registered & stock created successfully!", "success")
        return redirect("/company_login")

    return render_template("company_register.html")
if __name__ == "__main__":
    os.chdir(BASE_DIR)
    app.run(debug=True)
