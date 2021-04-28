
#pragma once

#include <string>

class Machine {


public:

  Machine();

  void enable_x_axis();

  void set_Axis_Configuration(bool _has_x, bool _has_y, bool _has_z = false , bool _has_b = false , bool _has_c = false);
  void set_X_Boundaries(double _min, double _max);
  void set_Y_Boundaries(double _min, double _max);
  void set_Z_Boundaries(double _min, double _max);
  void set_B_Boundaries(double _min, double _max);
  void set_C_Boundaries(double _min, double _max);

  void set_TCP_Configuration(bool _has_tcp);

  bool set_X(double _x);
  bool set_Y(double _y);
  bool set_Z(double _z);
  bool set_C(double _b);
  bool set_B(double _c);

  void enable_Sanity_Checks();

private:

   /** \brief is tool center point management enabled
    *
    */
   bool tcpEnabled_;

   bool has_tcp_;

   bool   has_x_axis_;
   double x_min_;
   double x_max_;
   double x_pos_;

   bool   has_y_axis_;
   double y_min_;
   double y_max_;
   double y_pos_;

   bool   has_z_axis_;
   double z_min_;
   double z_max_;
   double z_pos_;

   bool has_b_axis_;
   double b_min_;
   double b_max_;
   double b_pos_;

   bool has_c_axis_;
   double c_min_;
   double c_max_;
   double c_pos_;

   bool sanity_Check_;

   bool error_Encountered_;
   std::string error_message_;

};
