#ifndef CUSTOMER_H_ 
#define CUSTOMER_H_

#include <iostream>
#include <string>

#include "List.h"
#include "Order.h"
#include "User.h"
using namespace std;

class Order;
class Customer: public User {
private:
	string address;
	string city;
	unsigned zip;
	string email;
	subOrder sub;
	Order * r;	//store active order
	List<Order*> orders; //store history order

public:
	Customer();
	Customer(string username, string password, string firstName,
			string lastName, bool isEmployee, string address, string city,
			unsigned zip, string email);

	string getAddress() const;
	string getCity() const;
	unsigned getZip() const;
	string getEmail() const;

	void setAddress(string address);
	void setCity(string city);
	void setZip(unsigned zip);
	void setEmail(string email);

	string displayCustomer(); //print all customers
	string getOrder() const;		//print list of orders
	//void insertOrder();

	bool operator==(const Customer& customer);
	bool operator<(const Customer& customer);
	bool operator>(const Customer& customer);

	string toString();	//return customer's info without
						//order information

	void read(ifstream& in);	//read data from file
	void write(ostream& out);	//write data to file

	friend ostream& operator<<(ostream& out, const Customer& customer);
	void activeOrder(Order *p);	//pass in orders to the cart.
	string printActive();//return active order detail (Order's printDeTailed() )
	void placeOrder();		//place order
	void addToProduct(Product* p);	//add product to the active Order
	void removeProduct(int index);//remove product from cart if order not placed.
	void insertOrder(Order* o); //insert order when loading
};

#endif /* CUSTOMER_H_ */
