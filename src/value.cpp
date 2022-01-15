#include "value.hpp"

using namespace std;

string to_string(Value v) {
  switch(v.type) {
    case Value::VAL_BOOL:   return v.as.boolean ? "true" : "false";
    case Value::VAL_NIL:    return "nil";
    case Value::VAL_NUMBER: return to_string(v.as.number);
  }
  return nullptr;
}
