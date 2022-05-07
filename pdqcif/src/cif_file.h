

#ifndef ROW_CIF_FILE_
#define ROW_CIF_FILE_

#define _USE_MATH_DEFINES
#include <cmath>
// leave ^ include up there. Moving it down breaks the M_PI value

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
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
         bool all_empty{ true };

         LoopPair() 
            : tag{ }, values{  } {}

         //LoopPair(std::string&& t) 
         //   : tag{ std::move(t) }, values{  } { std::cout << "making looppair &&" << tag << std::endl; }

         LoopPair(const std::string& t) 
            : tag{ t }, values{  } {}

         LoopPair(std::string& t) 
            : tag{ t }, values{ } {}

         LoopPair(const std::string& t, const std::vector<std::string>& v) 
            : tag{ t }, values{ v } {}

         //LoopPair(std::string&& t, std::vector<std::string>&& v) 
         //   : tag{ std::move(t) }, values{ std::move(v) } { std::cout << "making looppair &&" << tag << ": " << values[0] << std::endl; }

         LoopPair(const LoopPair& lp)
            : tag{ lp.tag }, values{ lp.values }{}

         const std::vector<std::string>& get_values() const {
            return values;
         }
         const std::vector<std::string>& get_values(const std::string& t) const {
            if (t == tag) {
               return values;
            }
            else {
               throw std::out_of_range("Tag does not exist.");
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

         Loop(const Loop& loop)
            :lpairs{ loop.lpairs }, currentAppend{ loop.currentAppend }, totalValues{ loop.totalValues }{}
         
         const std::vector<std::string>& get_values(const std::string& t) const {
            for (const LoopPair& lp : lpairs) {
               if (lp.has_tag(t)) {
                  return lp.values;
               }
            }
            throw std::out_of_range("Tag does not exist.");
         }
         const std::vector<std::string>& get_value(const std::string& t) const {
            return get_values(t);
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

         bool has_tag(const std::string& t) const {
            size_t i;
            return has_tag(t, i);
         }

         size_t find_tag(const std::string& t) const {
            size_t i;
            has_tag(t, i);
            return i;
         }

         bool remove_item(const std::string& t) {
            return std::erase_if(lpairs, [&t](row::cif::LoopPair& lp) { return lp.has_tag(t); }) != 0;
         }

      };

      struct Pair {
         std::string tag{};
         // if the value is stored in a vector, then the return type for all values is a vector<string>
         //  if it is only one element, then just use that one. More than one, then you have a loop.
         //Trust me. It's easier this way.
         std::vector<std::string> value{};

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

         Pair(const Pair& pair)
            :tag{ pair.tag }, value{ pair.value } {}

         const std::vector<std::string>& get_value() const {
            return value;
         }

         const std::vector<std::string>& get_value(const std::string& t) const {
            if (has_tag(t)) {
               return value;
            }
            else {
               throw std::out_of_range("Tag not found.");
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

         explicit Item(LoopArg)
            : type{ ItemType::Loop }, loop{} {}

         explicit Item(std::string&& t)
            : type{ ItemType::Pair }, pair{ std::move(t) } {}
         
         explicit Item(std::string& t)
            : type{ ItemType::Pair }, pair{ t } {}

         Item(const std::string& t, const std::string& v)
            : type{ ItemType::Pair }, pair{ t, v } {}

         explicit Item(const std::string& t)
            : type{ ItemType::Pair }, pair{ t } {}

         bool has_tag(const std::string& t) const {
            if (is_pair()) {
               return pair.has_tag(t);
            }
            else if (is_loop()) {
               return loop.has_tag(t);
            }
            else {
               return false;
            }
         }

         const std::vector<std::string>& get_value(const std::string& t) const {
            if (is_pair()) {
               return pair.get_value(t);
            }
            else if (is_loop()) {
               return loop.get_value(t);
            }
            throw std::out_of_range("Tag not found.");
         }


         bool is_loop() const {
            return type == ItemType::Loop;
         }

         bool is_pair() const {
            return type == ItemType::Pair;
         }

         //Rule of five - I'm using a union, so I need to do all the manual memory management
         // I'm using a union because I want to be able to pass references to the data, rather than do it by value, as in std::variant.
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
         Item(Item&& o) noexcept
            : type{ o.type }  { // constructor
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
         std::string pd_block_id{};
         std::vector<Item> items{};
         std::unordered_set<std::string> tags{};
         
         explicit Block(const std::string& name) 
            : name{ name }, pd_block_id{ name } {}

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
               }
               else if (item.is_pair()) {
                  if (items[i].pair.has_tag(t)) {
                     idx.item = i;
                     return idx;
                  }
               }
            }
            return idx;
         }

         const std::vector<std::string>& get_value(const ItemIndex& idx) const {
            if (idx.item == SIZE_MAX) {
               throw std::out_of_range("Tag not found.");
            }
            else if (idx.loop == SIZE_MAX) { //it's a pair
               return items[idx.item].pair.value;
            }
            else { //it's a loop
               return items[idx.item].loop.lpairs[idx.loop].values;
            }
         }

         const std::vector<std::string>& get_value(const std::string& t) const {
            ItemIndex idx = find_tag(t);
            return get_value(idx);
         }

         bool is_loop(const std::string& t) const  {
            ItemIndex idx = find_tag(t);
            return  idx.loop != SIZE_MAX && idx.item != SIZE_MAX ;
         }
        
         bool is_pair(const std::string& t) const  {
            ItemIndex idx = find_tag(t);
            return idx.loop == SIZE_MAX && idx.item != SIZE_MAX;
         }

         bool remove_tag(const std::string& t) {
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
               //also need to check if the entire loop is empty!
               if (loop.lpairs.empty()) {
                  items.erase(items.begin() + idx.item);
               }
               return true;
            }
         }

         bool remove_loop_containing_tag(const std::string& t) {
            ItemIndex idx = find_tag(t);
            if (idx.item == SIZE_MAX || idx.loop == SIZE_MAX) {
               return false;
            }
            else { //it's a loop
               items.erase(items.begin() + idx.item);
               return true;
            }
         }
      };

      struct Cif {
         std::string source{};
         std::vector<Block> blocks{};
         std::vector<Item>* items_ = nullptr; //this is used to point to the items in the current block
         std::string* blockcode_ = nullptr; // this points to the name of the current block
         std::unordered_set<std::string> blockcodes{};

         const Block& get_block(std::string name) {
            for (const Block& block : blocks) {
               if (block.name == name)
                  return block;
            }
            throw std::out_of_range("Block not found.");
         }

         const Block& get_block_by_id(std::string id) {
            for (const Block& block : blocks) {
               if(block.pd_block_id == id)
                  return block;
            }
            throw std::out_of_range("pd_block_id not found.");
         }
      };



      /************************************************************************************
      *
      * Helper functions
      *
      *************************************************************************************/
      void print(const std::string& s);
      void print(const int s);
      void print(const std::vector<std::string>& v);
      void print(const Pair& p);
      void print(const LoopPair& lp);
      void print(const Loop& loop);
      void print(const Item& item);
      void print(const Block& block, const bool print_all_values=true);
      void print(const Cif& cif, const bool print_all_values = true);




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