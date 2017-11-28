/*
 * student.hpp
 *
 *  Created on: Nov 28, 2017
 *      Author: lifuxin
 */

#ifndef DPDK_MESSAGE_STUDENT_HPP_
#define DPDK_MESSAGE_STUDENT_HPP_
#include <stdio.h>
#include<cstring>

class Student
{
public:
	Student(int _age, char *_name);
	int age;
	char name[64];
};

Student::Student(int _age, char *_name):age(_age)
{
	memcpy(name, _name, sizeof(_name));
}



#endif /* DPDK_MESSAGE_STUDENT_HPP_ */
