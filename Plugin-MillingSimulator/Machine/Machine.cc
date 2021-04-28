
#include "Machine.hh"
#include <iostream>


Machine::Machine() :
tcpEnabled_(false),
has_tcp_(false),
has_x_axis_(false),
x_min_(0.0),
x_max_(0.0),
x_pos_(0.0),
has_y_axis_(false),
y_min_(0.0),
y_max_(0.0),
y_pos_(0.0),
has_z_axis_(false),
z_min_(0.0),
z_max_(0.0),
z_pos_(0.0),
has_b_axis_(false),
b_min_(0.0),
b_max_(0.0),
b_pos_(0.0),
has_c_axis_(false),
c_min_(0.0),
c_max_(0.0),
c_pos_(0.0),
sanity_Check_(false)
{

}

void Machine::set_Axis_Configuration(bool _has_x, bool _has_y, bool _has_z , bool _has_b , bool _has_c ) {
  has_x_axis_ = _has_x;
  has_y_axis_ = _has_y;
  has_z_axis_ = _has_z;
  has_b_axis_ = _has_b;
  has_c_axis_ = _has_c;
}


void Machine::set_X_Boundaries(double _min, double _max) {
  x_min_      = _min;
  x_max_      = _max;
  has_x_axis_ = true;
}

void Machine::set_Y_Boundaries(double _min, double _max) {
  y_min_      = _min;
  y_max_      = _max;
  has_y_axis_ = true;
}

void Machine::set_Z_Boundaries(double _min, double _max) {
  z_min_      = _min;
  z_max_      = _max;
  has_z_axis_ = true;
}

void Machine::set_B_Boundaries(double _min, double _max) {
  b_min_      = _min;
  b_max_      = _max;
  has_b_axis_ = true;
}

void Machine::set_C_Boundaries(double _min, double _max) {
  c_min_      = _min;
  c_max_      = _max;
  has_c_axis_ = true;
}

void Machine::set_TCP_Configuration(bool _has_tcp) {
  has_tcp_ = _has_tcp;
}

bool Machine::set_X(double _x) {
  if ( sanity_Check_ ) {
    if ( _x < x_min_ ) {
      error_message_ +=  " X Axis position out of Range! Minimum is : " + std::to_string(x_min_) + " Value to be set is : " + std::to_string(_x) ;
      error_Encountered_ = true;
      return false;
    }

    if ( _x > x_max_ ) {
      error_message_ +=  " X Axis position out of Range! Maximum is : " + std::to_string(x_max_) + " Value to be set is : " + std::to_string(_x) ;
      error_Encountered_ = true;
      return false;
    }
  }

  x_pos_ = _x;
  return true;
}

bool Machine::set_Y(double _y) {
  if ( sanity_Check_ ) {
    if ( _y < y_min_ ) {
      error_message_ +=  " Y Axis position out of Range! Minimum is : " + std::to_string(y_min_) + " Value to be set is : " + std::to_string(_y) ;
      error_Encountered_ = true;
      return false;
    }

    if ( _y > x_max_ ) {
      error_message_ +=  " Y Axis position out of Range! Maximum is : " + std::to_string(y_max_) + " Value to be set is : " + std::to_string(_y) ;
      error_Encountered_ = true;
      return false;
    }
  }

  y_pos_ = _y;
  return true;

}

bool Machine::set_Z(double _z) {
  if ( sanity_Check_ ) {
    if ( _z < z_min_ ) {
      error_message_ +=  " Z Axis position out of Range! Minimum is : " + std::to_string(z_min_) + " Value to be set is : " + std::to_string(_z) ;
      error_Encountered_ = true;
      return false;
    }

    if ( _z > z_max_ ) {
      error_message_ +=  " Z Axis position out of Range! Maximum is : " + std::to_string(z_max_) + " Value to be set is : " + std::to_string(_z) ;
      error_Encountered_ = true;
      return false;
    }
  }

  z_pos_ = _z;
  return true;

}

bool Machine::set_B(double _b) {
  if ( sanity_Check_ ) {
    if ( _b < b_min_ ) {
      error_message_ +=  " B Axis position out of Range! Minimum is : " + std::to_string(b_min_) + " Value to be set is : " + std::to_string(_b) ;
      error_Encountered_ = true;
      return false;
    }

    if ( _b > b_max_ ) {
      error_message_ +=  " B Axis position out of Range! Maximum is : " + std::to_string(b_max_) + " Value to be set is : " + std::to_string(_b) ;
      error_Encountered_ = true;
      return false;
    }
  }

  b_pos_ = _b;
  return true;
}


bool Machine::set_C(double _c) {
  if ( sanity_Check_ ) {
    if ( _c < c_min_ ) {
      error_message_ +=  " C Axis position out of Range! Minimum is : " + std::to_string(c_min_) + " Value to be set is : " + std::to_string(_c) ;
      error_Encountered_ = true;
      return false;
    }

    if ( _c > c_max_ ) {
      error_message_ +=  " C Axis position out of Range! Maximum is : " + std::to_string(c_max_) + " Value to be set is : " + std::to_string(_c) ;
      error_Encountered_ = true;
      return false;
    }
  }

  c_pos_ = _c;
  return true;

}



