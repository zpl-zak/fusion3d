#ifndef REFLECTION_H
#define REFLECTION_H
#include <string>
#include <map>
#include "factory.hpp"

#define NAME(x)  #x

#define REFLECT(p) 
#define CLASSTYPE _GetClassName_()

#define CLASS(p) class p\
{\
	inline virtual std::string _GetClassName_() { return #p; }\

#define COMPONENT(p) class p : public EntityComponent\
{\
	public:\
	p(){}\
	inline virtual std::string _GetClassName_() { return #p; }\

#define PROGRAM(p) class p : public Program\
{\
	virtual std::string GetProgramName() { return #p; }\



#endif