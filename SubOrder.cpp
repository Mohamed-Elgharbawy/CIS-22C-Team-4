/*
 * SubOrder.cpp 
 *
 *  Created on: Jun 17, 2018
 *      Author: Andrew
 */

#include "SubOrder.h"

    subOrder::subOrder() {
		laptop = NULL;
		price = 0;
		quantity = 1;
	}
    subOrder::subOrder(Product * toBuy) {
		laptop = toBuy;
		price = toBuy->getPrice();
		quantity = 1;
	}
	void subOrder::setQuantity(int q) {
		quantity = q;
		price = laptop -> getPrice() * q;
	}
	int subOrder::getQuantity() {
		return quantity;
	}
	ostream & operator<<(ostream & out, const subOrder& SO) {
		out << SO.laptop -> getMake() << "," <<  SO.laptop -> getModel() << "," << SO.quantity;
		return out;
	}
	bool subOrder::operator==(const subOrder &rhs) {
		return (*laptop == *rhs.laptop);
	}



