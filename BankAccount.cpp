#include "BankAccount.h"
#include <iostream>
#include <cstring>

Transaction::Transaction(int i,
                         double a,
                         const char* desc)
{
    id = i;
    amount = a;
    description = new char[strlen(desc) + 1]; // FIX #1: +1 for null terminator
    strcpy(description, desc);
}

Transaction::~Transaction()
{
    delete[] description; // FIX #2: delete[] instead of delete
}

BankAccount::BankAccount(int id,
                         std::string name,
                         double initialBalance)
{
    accountId = id;
    owner = name;
    balance = new double;
    *balance = initialBalance;
}

BankAccount::~BankAccount()
{
    delete balance;
    for(size_t i = 0; i < transactions.size(); i++) // FIX #3: changed <= to 
    {
        delete transactions[i];
    }
}

void BankAccount::deposit(double amount)
{
    std::lock_guard<std::mutex> lock(accountMutex); // FIX #11: lock for thread safety
    if(amount <= 0) // FIX #4: reject negative or zero deposits
    {
        std::cout << "Invalid deposit amount: must be positive\n";
        return;
    }
    *balance += amount;
}

void BankAccount::withdraw(double amount)
{
    std::lock_guard<std::mutex> lock(accountMutex); // FIX #11: lock for thread safety
    if(amount <= 0) // FIX #5: reject negative or zero withdrawals
    {
        std::cout << "Invalid withdrawal amount: must be positive\n";
        return;
    }
    if(*balance >= amount) // FIX #7: changed > to >= and added overdraft protection
    {
        *balance -= amount;
    }
    else
    {
        std::cout << "Insufficient funds for withdrawal\n";
    }
}

void BankAccount::transfer(BankAccount& target,
                           double amount)
{
    if(amount <= 0 || *balance < amount) // FIX #7: prevent overdraft on transfer
    {
        std::cout << "Transfer failed: insufficient funds or invalid amount\n";
        return;
    }
    withdraw(amount);
    target.deposit(amount);
}

double BankAccount::getBalance() const
{
    return *balance;
}

Transaction* BankAccount::getTransaction(int index)
{
    if(index < 0 || index >= static_cast<int>(transactions.size())) // FIX #10: bounds check
    {
        std::cout << "Invalid transaction index\n";
        return nullptr;
    }
    return transactions[index];
}

void BankAccount::addTransaction(Transaction* t)
{
    transactions.push_back(t);
}

void BankAccount::printStatement()
{
    for(unsigned int i = 0; i < transactions.size(); i++)
    {
        std::cout
            << transactions[i]->id
            << " "
            << transactions[i]->amount
            << " "
            << transactions[i]->description
            << std::endl;
    }
}

std::string BankAccount::generateReport() // FIX #6: returns std::string, no dangling pointer
{
    return "Account=" + std::to_string(accountId) +
           " Owner=" + owner +
           " Balance=" + std::to_string(*balance);
}
