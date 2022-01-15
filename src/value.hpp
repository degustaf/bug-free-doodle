#ifndef SRC_VALUE_HPP
#define SRC_VALUE_HPP

#include<string>

struct Value { 
  enum ValueType {           
    VAL_BOOL,              
    VAL_NIL,
    VAL_NUMBER,             
  };
  ValueType type;
  union {        
    bool boolean;
    double number;
  } as; 

  Value(bool b)   : type(VAL_BOOL)   {as.boolean = b;};
  Value(void)     : type(VAL_NIL)    {as.number = 0;};
  Value(double x) : type(VAL_NUMBER) {as.number = x;};
};

#define IS_BOOL(value)    ((value).type == Value::ValueType::VAL_BOOL)  
#define IS_NIL(value)     ((value).type == Value::ValueType::VAL_NIL)   
#define IS_NUMBER(value)  ((value).type == Value::ValueType::VAL_NUMBER)

#define AS_BOOL(value)    ((value).as.boolean)                       
#define AS_NUMBER(value)  ((value).as.number)

#define BOOL_VAL(value)   (Value(value)) 
#define NIL_VAL           (Value())       
#define NUMBER_VAL(value) (Value(value))

std::string to_string(Value v);

#endif /* SRC_VALUE_HPP */
