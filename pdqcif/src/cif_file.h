

#ifndef ROW_CIF_FILE_
#define ROW_CIF_FILE_

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


namespace row {
   namespace cif {

      struct ItemIndex {
         size_t item{ SIZE_MAX };
         size_t loop{ SIZE_MAX };

         ItemIndex() 
            : item{ SIZE_MAX }, loop{ SIZE_MAX } {}

         ItemIndex(size_t i, size_t j) 
            : item{ i }, loop{ j } {}

         ItemIndex(size_t i) 
            : item{ i }, loop{ SIZE_MAX } {}
      };

      struct LoopPair {
         std::string tag{};
         std::vector<std::string> values{};

         LoopPair() 
            : tag{ }, values{  } {}

         LoopPair(std::string&& t) 
            : tag{ std::move(t) }, values{  } {}

         LoopPair(const std::string& t) 
            : tag{ t }, values{  } {}

         LoopPair(std::string& t) 
            : tag{ t }, values{ } {}

         LoopPair(const std::string& t, const std::vector<std::string>& v) 
            : tag{ t }, values{ v } {}

         LoopPair(std::string&& t, std::vector<std::string>&& v) 
            : tag{ std::move(t) }, values{ std::move(v) } {}



         bool get_values(std::vector<std::string>* v) {
            v = &values;
            return true;
         }
         bool get_values(const std::string& t, std::vector<std::string>* v) {
            if (t == tag) {
               v = &values;
               return true;
            }
            else {
               v = nullptr;
               return false;
            }
         }

         bool has_tag(const std::string& t) const {
            return t == tag;
         }
      };

      struct Loop {
         std::vector<LoopPair> lpairs;
         size_t currentAppend{ 0 };
         size_t totalValues{ 0 };

         Loop()
            : lpairs{} {}

         Loop(std::vector<LoopPair> lps)
            : lpairs{ lps } {}

         bool get_values(const std::string& t, std::vector<std::string>* v) {
            for (LoopPair& lp : lpairs) {
               if (lp.has_tag(t)) {
                  v = &lp.values;
                  return true;
               }
            }
            v = nullptr;
            return false;
         }

         bool has_tag(const std::string& t) const {
            for (const LoopPair& lp : lpairs) {
               if (lp.has_tag(t))
                  return true;
            }
            return false;
         }

         bool has_tag(const std::string& t, size_t& ti) const {
            for (size_t i{ 0 }; i < lpairs.size(); ++i) {
               if (lpairs[i].has_tag(t)) {
                  ti = i;
                  return true;
               }
            }
            ti = SIZE_MAX;
            return false;
         }

         size_t find_tag(const std::string& t) const {
            for (size_t i{ 0 }; i < lpairs.size(); ++i) {
               if (lpairs[i].has_tag(t))
                  return i;
            }
            return SIZE_MAX;
         }

         bool remove_item(const std::string& t) {
            size_t i = find_tag(t);
            if (i == SIZE_MAX) return false;

            lpairs.erase(lpairs.begin() + i);
            return true;
         }

      };

      struct Pair {
         std::string tag{};
         std::string value{};

         Pair() : tag{ }, value{ } {}

         Pair(std::string&& t) 
            : tag{ std::move(t) }, value{ } {}

         Pair(const std::string& t) 
            : tag{ t }, value{ } {}

         Pair(const std::string&& t) 
            : tag{ t }, value{ } {}

         Pair(std::string& t) 
            : tag{ t }, value{ } {}

         Pair(const std::string& t, const std::string& v) 
            : tag{ t }, value{ v } {}

         bool get_value(std::string* v) {
            v = &value;
            return true;
         }

         bool get_value(const std::string& t, std::string* v) {
            if (t == tag) {
               v = &value;
               return true;
            }
            else {
               v = nullptr;
               return false;
            }
         }

         bool has_tag(const std::string& t) const {
            return t == tag;
         }
      };

      struct LoopArg {}; // used only as arguments when creating Item

      enum class ItemType : unsigned char {
         Pair,
         Loop
      };


      struct Item {
         ItemType type;
         union {
            Pair pair;
            Loop loop;
         };

         Item(LoopArg)
            : type{ ItemType::Loop }, loop{} {}

         Item(std::string&& t) 
            : type{ ItemType::Pair }, pair{ std::move(t) } {}
         
         Item(std::string& t)
            : type{ ItemType::Pair }, pair{ t } {}

         Item(const std::string& t, const std::string& v)
            : type{ ItemType::Pair }, pair{ t, v } {}

         Item(const std::string& t)
            : type{ ItemType::Pair }, pair{ t } {}

         bool get_value(const std::string& t, std::vector<std::string>* v) {
            size_t i{ SIZE_MAX };
            if (type == ItemType::Loop && loop.has_tag(t, i)) {
               v = &loop.lpairs[i].values;
               return true;
            }
            else {
               v = nullptr;
               return false;
            }
         }

         bool get_value(const std::string& t, std::string* v) {
            if (type == ItemType::Pair && pair.has_tag(t)) {
               v = &pair.value;
               return true;
            }
            else {
               v = nullptr;
               return false;
            }
         }

         bool has_tag(const std::string& t) const {
            if (type == ItemType::Pair) {
               return pair.has_tag(t);
            }
            else if (type == ItemType::Loop) {
               return loop.has_tag(t);
            }
            else {
               return false;
            }
         }

         bool is_loop() const {
            return type == ItemType::Loop;
         }

         bool is_pair() const {
            return type == ItemType::Pair;
         }

         //Rule of five - I'm using a union, so I need to do all the maual memory management
         // I'm using a union because I want to be able to pass references to the data, rather than do it by value.
         // see https://en.cppreference.com/w/cpp/language/rule_of_three, https://riptutorial.com/cplusplus/example/5421/rule-of-five
         ~Item() { //destructor
            destroy();
         }
         //copy
         Item(const Item& o) // constructor
            : type{ o.type } {
            copyItem(o);
         }
         Item& operator=(const Item& o) { // assignment
            if (o.type == type) {
               copyItem(o);
            }
            else {
               destroy();
               type = o.type;
               copyItem(o);
            }
            return *this;
         }
         //move
         Item(Item&& o) noexcept { // constructor
            moveItem(std::move(o));
         }
         Item& operator=(Item&& o) noexcept { // assignement
            if (o.type == type) {
               moveItem(std::move(o));
            }
            else {
               destroy();
               type = o.type;
               moveItem(std::move(o));
            }
            return *this;
         }

      private:
         void destroy() {
            switch (type) {
            case ItemType::Pair: pair.~Pair(); break;
            case ItemType::Loop: loop.~Loop(); break;
            }
         }

         void copyItem(const Item& o) {
            switch (type) {
            case ItemType::Pair: new (&pair) Pair(o.pair); break; //placement new operator. It reads as:
            case ItemType::Loop: new (&loop) Loop(o.loop); break; // at the address of loop, make a copy of o.loop using the copy constructor
            }
         }

         void moveItem(Item&& o) {
            switch (type) {
            case ItemType::Pair: new (&pair) Pair(std::move(o.pair)); break; //placement new operator. It reads as:
            case ItemType::Loop: new (&loop) Loop(std::move(o.loop)); break; // at the address of loop, move o.loop using the move constructor
            }
         }

      };

      struct Block {
         std::string name{};
         std::vector<Item> items{};

         Block(const std::string& name) 
            : name{ name }, items{} {}

         bool has_tag(const std::string& t) const {
            for (const Item& item : items) {
               if (item.has_tag(t))
                  return true;
            }
            return false;
         }

         ItemIndex find_tag(const std::string& t) const {
            ItemIndex idx{};
            for (size_t i{ 0 }; i < items.size(); ++i) {
               const Item& item = items[i];
               if (item.is_loop()) {
                  idx.loop = items[i].loop.find_tag(t);
                  if (idx.loop != SIZE_MAX) {
                     idx.item = i;
                     return idx;
                  }
                  else if (item.is_pair()) {
                     if (items[i].pair.has_tag(t)) {
                        idx.item = i;
                        return idx;
                     }
                  }
               }
            }
            return idx;
         }

         bool get_value(const ItemIndex& idx, std::string* v) {
            if (idx.item == SIZE_MAX || idx.loop != SIZE_MAX) {
               v = nullptr;
               return false;
            }
            else {
               assert(items[idx.item].is_pair());
               v = &items[idx.item].pair.value;
               return true;
            }
         }

         bool get_value(const ItemIndex& idx, std::vector<std::string>* v) {
            if (idx.item == SIZE_MAX || idx.loop == SIZE_MAX) {
               v = nullptr;
               return false;
            }
            else {
               assert(items[idx.item].is_loop());
               v = &items[idx.item].loop.lpairs[idx.loop].values;
               return true;
            }
         }

         bool remove_item(const std::string& t) {
            ItemIndex idx = find_tag(t);
            if (idx.item == SIZE_MAX) {
               return false;
            }
            else if (idx.loop == SIZE_MAX) { //it's a Pair
               items.erase(items.begin() + idx.item);
               return true;
            }
            else { //it's a loop
               Loop& loop = items[idx.item].loop;
               loop.lpairs.erase(loop.lpairs.begin() + idx.loop);
               return true;
            }
         }

      };

      struct Cif {
         std::string source{};
         std::vector<Block> blocks{};
         std::vector<Item>* items_ = nullptr; //this is used to point to the items in the current block
         std::string* blockcode_ = nullptr; // this points to the name of the current block
      };



      /************************************************************************************
      *
      * Helper functions
      *
      *************************************************************************************/
      void print(const std::string& s) {
         std::cout << s << std::endl;
      }
      void print(const int s) {
         std::cout << s << std::endl;
      }
      void print(const std::vector<std::string>& v) {
         for (const std::string& s : v)
            std::cout << s << '\n';
         std::cout << std::endl;
      }
      void print(const Pair& p) {
         std::cout << p.tag << '\t' << p.value << std::endl;
      }
      void print(const LoopPair& lp) {
         std::cout << lp.tag << '\n';
         for (const std::string& v : lp.values)
            std::cout << '\t' << v << '\n';
         std::cout << std::endl;
      }
      void print(const Loop& loop) {
         size_t total_values = loop.lpairs.size() * loop.lpairs[0].values.size();
         size_t total_tags = loop.lpairs.size();
         //tags
         for (const LoopPair& p : loop.lpairs) {
            std::cout << p.tag << '\t';
         }
         //values
         for (size_t k = 0; k < total_values; ++k) {
            size_t col = k % total_tags;
            size_t row = k / total_tags;
            if (col == 0)
               std::cout << std::endl;
            std::cout << loop.lpairs[col].values[row] << '\t';
         }
         std::cout << std::endl;
      }
      void print(const Item& item) {
         if (item.type == ItemType::Pair)
            print(item.pair);
         else //should just be a Loop
            print(item.loop);
      }
      void print(const Block& block, const bool block_name_only) {
         std::cout << "---------------\nBlock:\t";
         std::cout << block.name << std::endl;
         if (!block_name_only) {
            for (const Item& item : block.items) {
               std::cout << "---------------\n";
               print(item);
            }
         }
         std::cout << "---------------\n" << std::endl;
      }
      void print(const Cif& cif, const bool block_name_only) {
         std::cout << "###############\nCIF:\t";
         std::cout << cif.source << std::endl;
         for (const Block& block : cif.blocks) {
            std::cout << "###############\n";
            print(block, block_name_only);
         }
         std::cout << "###############\n" << std::endl;
      }




   } //end namespace cif
} // end namespace row

#if 0
namespace cif::helper {

   //takes a string representing a number and gives two doubles containing the value and error. The function returns true.
   // If there is not an error, then the error is given as the sqrt of the value. The function returns false.
   bool split_val_err(const std::string& ve, double& v, double& e) {
      size_t fb = ve.find('(');
      if (fb == std::string::npos) { //there is no error in the string
         if (ve == "." || ve == "?") {
            v = std::numeric_limits<double>::quiet_NaN();
            e = std::numeric_limits<double>::quiet_NaN();
         }
         else {
            v = std::stod(ve);
            e = sqrt(abs(v));
         }
         return false;
      }

      size_t lb = ve.find(')');
      std::string val = ve.substr(0, fb);
      std::string err = ve.substr(fb + 1, lb - fb - 1);
      size_t d = val.find('.');
      int pow{ 0 };

      if (d != std::string::npos) {
         pow = static_cast<int>(val.size() - d) - 1;
      }

      v = std::stod(val);
      e = std::stod(err) / std::pow(10, pow);
      return true;
   }

   void split_val_err(const std::vector<std::string>& ves, std::vector<double>& v, std::vector<double>& e) {
      double val{ 0 };
      double err{ 0 };

      for (const std::string ve : ves) {
         split_val_err(ve, val, err);
         v.push_back(val);
         e.push_back(err);
      }
   }
}
#endif


#endif