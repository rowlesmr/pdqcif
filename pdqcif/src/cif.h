

#ifndef CIF
#define CIF

#define _USE_MATH_DEFINES
#include <cmath>
// leave ^ include up there. Moving it down breaks the M_PI value

#include <iostream>
#include <string>
#include <vector>
#include <variant>
//#include <stdexcept>

#include <limits>
#include <assert.h>

#include "cif_rules.h"

namespace cif {

   struct ItemIndex {
      size_t item{SIZE_MAX}; 
      size_t loop{SIZE_MAX};

      ItemIndex();
      ItemIndex(size_t i, size_t j);
      ItemIndex(size_t i);
   };

   struct LoopPair {
      std::string tag{};
      std::vector<std::string> values{};

      LoopPair();
      LoopPair(std::string&& t);
      LoopPair(const std::string& t);
      LoopPair(std::string& t);
      LoopPair(const std::string& t, const std::vector<std::string>& v);
      LoopPair(std::string&& t, std::vector<std::string>&& v);

      bool get_values(std::vector<std::string>* v);
      bool get_values(const std::string& t, std::vector<std::string>* v);
      bool has_tag(const std::string& t) const;
   };

   struct Loop {
      std::vector<LoopPair> lpairs;
      size_t currentAppend{ 0 };
      size_t totalValues{ 0 };

      Loop();

      bool get_values(const std::string& t, std::vector<std::string>* v);
      bool has_tag(const std::string& t) const;
      bool has_tag(const std::string& t, size_t& ti) const;
      size_t find_tag(const std::string& t) const;
      bool remove_item(const std::string& t);
   };

   struct Pair {
      std::string tag{};
      std::string value{};

      Pair();
      Pair(std::string&& t);
      Pair(const std::string& t);
      Pair(const std::string&& t);
      Pair(std::string& t);
      Pair(const std::string& t, const std::string& v);
   
      bool get_value(std::string* v);
      bool get_value(const std::string& t, std::string* v);
      bool has_tag(const std::string& t) const;
   };

   struct LoopArg; // used only as arguments when creating Item

   struct Item {
      std::variant<Pair, Loop> data;

      Item(LoopArg);
      Item(std::string&& t);
      Item(std::string& t);
      Item(const std::string& t, const std::string& v);
      Item(const std::string& t);

      bool get_value(const std::string& t, std::vector<std::string>* v);
      bool get_value(const std::string& t, std::string* v);
      bool has_tag(const std::string& t) const;
   };

   struct Block {
      std::string name{};
      std::vector<Item> items{};

      Block(const std::string& name);

      bool has_tag(const std::string& t) const;
      ItemIndex find_tag(const std::string& t) const;
      bool get_value(const ItemIndex& idx, std::string* v);
      bool get_value(const ItemIndex& idx, std::vector<std::string>* v);
      bool remove_item(const std::string& t);
   };

   struct Cif {
      std::string source{};
      std::vector<Block> blocks{};
      std::vector<Item>* items_ = nullptr; //this is used to point to the items in the current block
      std::string* blockcode_ = nullptr; // this points to the name of the current block
   };

   void print(const std::string& s);
   void print(const int s);
   void print(const std::vector<std::string>& v);
   void print(const Pair& p);
   void print(const LoopPair& lp);
   void print(const Loop& loop);
   void print(const Item& item);
   void print(const Block& block, const bool block_name_only = false);
   void print(const Cif& cif, const bool block_name_only = false);
   

   template<typename Input> void parse_input(Cif& d, Input&& in);

   template<typename Input> Cif read_input(Input&& in);

} //end namespace cif



namespace cif::helper {

   //takes a string representing a number and gives two doubles containing the value and error. The function returns true.
   // If there is not an error, then the error is given as the sqrt of the value. The function returns false.
   bool split_val_err(const std::string& ve, double& v, double& e);
   void split_val_err(const std::vector<std::string>& ves, std::vector<double>& v, std::vector<double>& e);
}


#endif