/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef DERIVEDA_H
#define DERIVEDA_H

#include "Base.h"

#include <iostream>

using namespace std;

class DerivedA : public Base{

private:
  static  DerivedRegister<DerivedA> reg;

public:
  virtual void print() {
  
    cout << "I am DerivedA" << endl;
  }
};

#endif // DERIVEDA_H
