#include <iostream>
#include <map>
#include <string>
#include <stdlib.h>


using namespace std;


//***********************
// Account Number ADT
//***********************

class AccountNo {                       /* Value range: 0001-9999 */
public:
    AccountNo ();                       /* New account number                          */
    explicit AccountNo ( int number );  /* Existing account number (for lookups)       */
    int number () const;                /* Accessor -- integer value of account number */
private:
    int number_;
    static int next_;
    static int const maxVal_ = 9999;
};


// initialization of static data member to the lowest legal value
int AccountNo::next_ = 1;


// constructor -- constructs a new unique account number
AccountNo::AccountNo () {
    if ( next_ > maxVal_ )
        exit (1);
    number_ = next_++;
}

// constructor -- converts an integer into an account number
// REQUIRES:  number corresponds to an existing account number
AccountNo::AccountNo ( int number ) {
    if ( (number > maxVal_) || (number < 1) )
        exit(1);
    number_ = number;
}


// accessor - returns account number value of object
int AccountNo::number() const {
    return number_;
}


// comparison operators
bool operator== (const AccountNo &a, const AccountNo &b) {
    return a.number() == b.number();
}

bool operator!= (const AccountNo &a, const AccountNo &b) {
    return !(a==b);
}

bool operator< (const AccountNo &a, const AccountNo &b) {
    return a.number() < b.number();
}

bool operator<= (const AccountNo &a, const AccountNo &b) {
    return (a<b) || (a==b);
}

bool operator> (const AccountNo &a, const AccountNo &b) {
    return !(a<=b);
}

bool operator>= (const AccountNo &a, const AccountNo &b) {
    return !(a<b);
}


// streaming operators
istream& operator>> ( istream &sin, AccountNo &a ) {
    int number;
    sin >> number;
    a = AccountNo(number);

    return sin;
}

ostream& operator<< ( ostream &sout, const AccountNo &a ) {
    sout.setf(ios::internal);
    sout.width(4);
    sout.fill('0');
    sout << a.number();

    return sout;
}


//*******************
// Account
//*******************

class Account {
public:                                         // PUBLIC interface of Account
    explicit Account( const AccountNo& );           // constructor
    virtual ~Account() {}                           // destructor
    int balance () const;                           // accessor - returns balance as an integer
    virtual void call ( int duration ) = 0;         // records information about a call (e.g., duration of call in minutes)
    virtual void bill () = 0;                       // decrements balance by monthly fee and the cost of using extra minutes
    void pay (int amount);                          // increments balance by amount paid
    virtual void print() const;                     // prints information about the account (e.g., account number, balance, minutes used this month)
protected:
    void balanceIs( const int );                    // mutator - changes balance of the account
private:
    AccountNo const accountNo_;
    int balance_;
};

// constructor -- constructs a new account with an automatically generated account number and a balance of zero
Account::Account(const AccountNo &accountNo) : accountNo_(accountNo), balance_(0) { }

// accessor - returns balance value of object
int Account::balance() const {
    return balance_;
}

// increments balance value of object by the amount
void Account::pay(int amount) {
    balance_ += amount;
}

// prints the account number and balance values of the object
void Account::print() const {
    cout << "  Account Number = " << accountNo_ << endl;
    cout << "  Balance = " << (balance_ < 0 ? "-" : "") << "$" << abs(balance_) << endl;
}

// mutator - changes the balance value of object to the new balance
void Account::balanceIs(const int newBalance) {
    balance_ = newBalance;
}


//*******************
// CheapAccount
//*******************

class CheapAccount : public Account {           //  Derived class of Account for Cheap Plan
public:
    explicit CheapAccount( const AccountNo& );      // constructor
    void call( int duration );                      // records information about a call (e.g., duration of call in minutes)
    void bill();                                    // decrements balance by monthly fee and the cost of using extra minutes
    void print() const;                             // prints information about the Cheap Plan Account (e.g., account number, balance, minutes used this month)
private:
    int minutes_;
    static int const monthlyCharge_ = 30;
    static int const freeMinutes_ = 200;
    static int const chargePerMinute_ = 1;
};

// constructor -- constructs a new Cheap Account with a base Account object and a balance of zero
CheapAccount::CheapAccount(const AccountNo &accountNo) : Account(accountNo), minutes_(0) { }

// increments the minute value of object by duration of call
void CheapAccount::call(int duration) {
    minutes_ += duration;
}

// decrements balance value of object by monthlyCharge and the cost of using extra minutes,
// and changes the minutes value of object to zero
void CheapAccount::bill() {
    int newBalance = Account::balance() - monthlyCharge_;
    if (minutes_ > freeMinutes_) {
        newBalance -= (minutes_ - freeMinutes_)*chargePerMinute_;
    }
    Account::balanceIs(newBalance);
    minutes_ = 0;
}

// prints type of account, information about the base account object and minutes value of the object
void CheapAccount::print() const {
    cout << "CheapAccount:" << endl;
    Account::print();
    cout << "  Minutes = " << minutes_ << endl;
}


//*******************
// ExpensiveAccount
//*******************

class ExpensiveAccount : public Account {           // Derived class of Account for Expensive Plan
public:
    explicit ExpensiveAccount( const AccountNo& );      // constructor
    void call( int duration ) {}                        // records information about a call
    void bill();                                        // decrements balance by monthly fee
    void print() const;                                 // prints information about the Expensive Plan Account (e.g., account number, balance)
private:
    static int const monthlyCharge_ = 100;
};

// constructor - constructs a new Expensive Account with a base Account object
ExpensiveAccount::ExpensiveAccount(const AccountNo &accountNo) : Account(accountNo) { }

// decrements balance value of object by monthlyCharge,
// and changes the minutes value of object to zero
void ExpensiveAccount::bill() {
    Account::balanceIs(Account::balance() - monthlyCharge_);
}

// prints type of account and information about the base account object
void ExpensiveAccount::print() const {
    cout << "ExpensiveAccount:" << endl;
    Account::print();
}


//************************************************************************
//  Helper variables and functions for test harness
//************************************************************************

//  test-harness operators
enum Op { NONE, NewE, NewC, Balance, Call, Bill, Pay, PrintAll };


//  converts a one-character input comment into its corresponding test-harness operator
Op convertOp( string opStr ) {
    switch( opStr[0] ) {
        case 'E': return NewE;
        case 'C': return NewC;
        case 'b': return Balance;
        case 'c': return Call;
        case 'B': return Bill;
        case 'p': return Pay;
        case 'P': return PrintAll;
        default: {
            cerr << "Invalid operation " << opStr << endl;
            return NONE;
        }
    }
}


// Reads anumber from cin and finds the corresponding Phone Service account
// REQUIRES: the next word to read from cin is an integer
// RETURNS: a pointer to a found account.  Otherwise, returns NULL
Account* findAccount( map<AccountNo,Account*>  &accounts) {
    int num;
    cin >> num;
    AccountNo act( num );
    map<AccountNo,Account*>::iterator iter;
    iter = accounts.find( act );
    if ( iter == accounts.end() ) {
        cerr << "Invalid Account Number!" << endl;
        return NULL;
    }
    return iter->second;
}


//*******************
// main()
//*******************

int main () {
    cout << "Test harness for family of phone-service accounts:" << endl << endl;
    map<AccountNo, Account*> accounts;

    cout << "Command: ";
    string command;
    cin >> command;

    Op op = convertOp(command);

    while ( !cin.eof() ) {
        switch ( op ) {
            /* Constructors */
            case NewE: {
                AccountNo act;
                Account* p = new ExpensiveAccount( act );
                accounts.insert( pair<AccountNo,Account*>( act, p ) );
                p->print();
                break;
            }
            case NewC: {
                AccountNo act;
                Account* p = new CheapAccount( act );
                accounts.insert( pair<AccountNo,Account*>( act, p ) );
                p->print();
                break;
            }

                /* Accessors */
            case Balance: {
                Account* p = findAccount( accounts );
                if ( p!= NULL)
                    cout << "Value of balance data member is: " << p->balance() << endl;
                break;
            }

                /* Phone Service Operations */
            case Call: {
                Account* p = findAccount( accounts );
                if ( p != NULL ) {
                    int duration;
                    cin >> duration;
                    p->call( duration );
                }
                break;
            }


            case Bill: {
                map<AccountNo,Account*>::iterator iter;
                map<AccountNo,Account*>::iterator end = accounts.end();
                for ( iter = accounts.begin(); iter != end; iter++ ) {
                    (iter->second)->bill();
                }
                break;
            }


            case Pay: {
                Account* p = findAccount( accounts );
                if (p != NULL ) {
                    int amt;
                    cin >> amt;
                    p->pay(amt);
                }
                break;
            }


                /* Print Accounts */
            case PrintAll: {
                map<AccountNo,Account*>::iterator iter;
                map<AccountNo,Account*>::iterator end = accounts.end();
                for ( iter = accounts.begin(); iter != end; iter++ ) {
                    (iter->second)->print();
                }
                break;
            }
            default: {
                break;
            }
        } // switch

        cout << endl << "Command: ";
        cin >> command;
        op = convertOp(command);

    } // while cin OK

    map<AccountNo,Account*>::iterator iter;
    map<AccountNo,Account*>::iterator end = accounts.end();
    for ( iter = accounts.begin(); iter != end; iter++ )
        delete iter->second;

    return 0;
}