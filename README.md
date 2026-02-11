# 📈 Stock Simulator – OOP Trading System (C++ + Flask)

A modular stock market simulation platform built using **Object Oriented Programming in C++**, supporting trader/company accounts, transactions, portfolio tracking, SIP investment modeling, and leaderboard ranking.

The core trading engine is written fully in **C++**, with persistent storage using JSON files.  
A lightweight **Flask** layer is used to trigger C++ execution and display outputs through a browser-based interface.

This project demonstrates how low-level systems can be integrated into higher-level application workflows.

---

## 🚀 Features

### 👤 Trader Capabilities
- Register & login with password validation  
- Deposit / withdraw balance  
- Buy & sell stocks  
- View portfolio & investment history  
- Track holdings and valuation  
- Update password  
- SIP investment calculator  
- Appear in global leaderboard

### 🏢 Company Capabilities
- Company account creation  
- Set initial valuation  
- Raise stock supply  
- Modify SIP return rate  
- Update company valuation  
- View company profile

### 🏆 System
- Market view for all traders  
- Persistent storage using JSON  
- Ranking via leaderboard  
- Modular class-based design  
- CLI engine + optional web interface

---

## 🧠 Architecture Overview

```
Frontend (Flask + HTML)
        ↓
   Executes C++ Engine
        ↓
 Business Logic (OOP Classes)
        ↓
     JSON Storage
```

---

## 🧩 Core Design

The system is decomposed into independent modules:

- `Trader` → account management, transactions, portfolio
- `Company` → stock issuance, valuation, SIP rate
- `Stock` → market entities
- `Leaderboard` → ranking logic
- `SIP` → return computation
- JSON files → persistence layer

This separation makes the engine scalable and easy to extend.

---

## 🛠 Tech Stack

**Backend Engine**
- C++
- OOP principles
- STL
- File handling
- JSON persistence

**Web Layer**
- Python
- Flask
- HTML / templates

---

## 📂 Project Structure

```
.
├── trader.cpp / trader.h
├── company.cpp / company.h
├── stock.cpp / stock.h
├── leaderboard.cpp / leaderboard.h
├── server.py              # Flask interface
├── templates/             # HTML pages
├── Traders/               # Trader data (JSON)
├── companies/             # Company data (JSON)
```

---

## ⚙️ How It Works

1. Traders and companies register.
2. Data is stored in structured JSON files.
3. All buy/sell/deposit operations update persistent state.
4. Leaderboard computes rankings dynamically.
5. Flask triggers the C++ program and displays output to users.

---

## ▶️ Running the Project

### Option 1 — CLI Mode
Compile and run:

```bash
g++ main.cpp trader.cpp company.cpp stock.cpp leaderboard.cpp -o market
./market
```

---

### Option 2 — Web Mode (Flask wrapper)

```bash
python server.py
```

Open browser → `http://localhost:5000`

---

## 🔐 Password Rules
Passwords must:
- Be longer than 8 characters  
- Include uppercase letter  
- Include digit  
- Include symbol  

---

## 💡 Engineering Highlights

- Built full trading workflow from scratch  
- Applied encapsulation & class responsibility  
- Managed state across sessions via JSON  
- Integrated system-level code with web layer  
- Designed menu-driven extensible architecture  

---

## 🧪 Possible Extensions
- REST API instead of CLI execution  
- Database migration (PostgreSQL / MongoDB)  
- Live market simulation  
- Authentication tokens  
- Graphical analytics  
- Concurrency handling  

---

## 🎯 Why this project matters

This project reflects real-world backend engineering:

✔ account management  
✔ financial logic  
✔ persistent state  
✔ modular design  
✔ integration across technologies  

---

## 👨‍💻 Author

Rajveer Gupta  
B.Tech CSE – IIIT Nagpur  

---

⭐ If you like the project, consider giving it a star!
