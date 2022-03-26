

#include "cif.h"

namespace cif {
   /************************************************************************************
   *
   * ItemIndex
   *
   *************************************************************************************/
   ItemIndex::ItemIndex() : item{ SIZE_MAX }, loop{ SIZE_MAX } {}
   ItemIndex::ItemIndex(size_t i, size_t j) : item{ i }, loop{ j } {}
   ItemIndex::ItemIndex(size_t i) : item{ i }, loop{ SIZE_MAX } {}

   /************************************************************************************
   *
   * LoopPair
   * 
   *************************************************************************************/
   LoopPair::LoopPair() : tag{ }, values{  } {}
   LoopPair::LoopPair(std::string&& t) : tag{ std::move(t) }, values{  } {}
   LoopPair::LoopPair(const std::string& t) : tag{ t }, values{  } {}
   LoopPair::LoopPair(std::string& t) : tag{ t }, values{ } {}
   LoopPair::LoopPair(const std::string& t, const std::vector<std::string>& v) : tag{ t }, values{ v } {}
   LoopPair::LoopPair(std::string&& t, std::vector<std::string>&& v) : tag{ std::move(t) }, values{ std::move(v) } {}
   
   bool LoopPair::get_values(std::vector<std::string>* v) {
      v = &values;
      return true;
   }

   bool LoopPair::get_values(const std::string& t, std::vector<std::string>* v) {
      if (t == tag) {
         v = &values;
         return true;
      }
      else {
         v = nullptr;
         return false;
      }
   }

   bool LoopPair::has_tag(const std::string& t) const {
      return t == tag;
   }

   /************************************************************************************
   *
   * Loop
   *
   *************************************************************************************/
   Loop::Loop() :lpairs{} {}

   bool Loop::get_values(const std::string& t, std::vector<std::string>* v) {
      for (LoopPair& lp : lpairs) {
         if (lp.has_tag(t)) {
            v = &lp.values;
            return true;
         }
      }
      v = nullptr;
      return false;
   }

   bool Loop::has_tag(const std::string& t) const {
      for (const LoopPair& lp : lpairs) {
         if (lp.has_tag(t))
            return true;
      }
      return false;
   }

   bool Loop::has_tag(const std::string& t, size_t& ti) const {
      for (size_t i{ 0 }; i < lpairs.size(); ++i) {
         if (lpairs[i].has_tag(t)) {
            ti = i;
            return true;
         }  
      }
      ti = SIZE_MAX;
      return false;
   }

   size_t Loop::find_tag(const std::string& t) const {
      for (size_t i{ 0 }; i < lpairs.size(); ++i) {
         if (lpairs[i].has_tag(t))
            return i;
      }
      return SIZE_MAX;
   }

   bool Loop::remove_item(const std::string& t) {
      size_t i = find_tag(t);
      if (i == SIZE_MAX) return false;

      lpairs.erase(lpairs.begin() + i);
      return true;
   }

   /************************************************************************************
   *
   * Pair
   *
   *************************************************************************************/
   Pair::Pair() : tag{ }, value{ } {}
   Pair::Pair(std::string&& t) : tag{ std::move(t) }, value{ std::string() } {}
   Pair::Pair(const std::string& t) : tag{ t }, value{ std::string() } {}
   Pair::Pair(const std::string&& t) : tag{ t }, value{ std::string() } {}
   Pair::Pair(std::string& t) : tag{ t }, value{ std::string() } {}
   Pair::Pair(const std::string& t, const std::string& v) : tag{ t }, value{ v } {}
      
   bool Pair::get_value(std::string* v) {
      v = &value;
      return true;
   }

   bool Pair::get_value(const std::string& t, std::string* v) {
      if (t == tag) {
         v = &value;
         return true;
      }
      else {
         v = nullptr;
         return false;
      }
   }

   bool Pair::has_tag(const std::string& t) const {
      return t == tag;
   }

   /************************************************************************************
   *
   * LoopArg
   *
   *************************************************************************************/
   //struct LoopArg {}; // used only as arguments when creating Item

   /************************************************************************************
   *
   * Item
   *
   *************************************************************************************/
   Item::Item(LoopArg) {
      data = Loop();
   }
   Item::Item(std::string&& t) {
      data = Pair(t);
   }
   Item::Item(std::string& t) {
      data = Pair(t);
   }
   Item::Item(const std::string& t, const std::string& v) {
      data = Pair(t, v);
   }
   Item::Item(const std::string& t) {
      data = Pair(t);
   }

   bool Item::get_value(const std::string& t, std::vector<std::string>* v) {
      if (std::holds_alternative<::cif::Loop>(data) && std::get<::cif::Loop>(data).has_tag(t)) {
         size_t i = std::get<::cif::Loop>(data).find_tag(t);
         v = &std::get<::cif::Loop>(data).lpairs[i].values;
         return true;
      }
      else {
         v = nullptr;
         return false;
      }
   }

   bool Item::get_value(const std::string& t, std::string* v) {
      if (std::holds_alternative<::cif::Pair>(data) && std::get<::cif::Pair>(data).has_tag(t)) {
         v = &std::get<::cif::Pair>(data).value;
         return true;
      }
      else {
         v = nullptr;
         return false;
      }
   }

   bool Item::has_tag(const std::string& t) const {
      if (std::holds_alternative<::cif::Pair>(data)) {
         return std::get<::cif::Pair>(data).has_tag(t);
      }
      else if (std::holds_alternative<::cif::Loop>(data)) {
         return std::get<::cif::Loop>(data).has_tag(t);
      }
      else {
         return false;
      }
   }

   /************************************************************************************
   *
   * Block
   *
   *************************************************************************************/
   Block::Block(const std::string& name) : name{ name }, items{} {}

   bool Block::has_tag(const std::string& t) const {
      for (const Item& item : items) {
         if (item.has_tag(t))
            return true;
      }
      return false;
   }

   ItemIndex Block::find_tag(const std::string& t) const {
      ItemIndex idx{};
      for (size_t i{ 0 }; i < items.size(); ++i) {
         const Item& item = items[i];
         if (std::holds_alternative<::cif::Loop>(item.data)) {
            const Loop& loop = std::get<::cif::Loop>(item.data);
            idx.loop = loop.find_tag(t);
            if (idx.loop != SIZE_MAX) {
               idx.item = i;
               return idx;
            }
            else if (std::holds_alternative<::cif::Pair>(item.data)){
               const Pair& pair = std::get<::cif::Pair>(item.data);
               if (pair.has_tag(t)) {
                  idx.item = i;
                  return idx;
               }
            }
         }
      }
      return idx;
   }

   bool Block::get_value(const ItemIndex& idx, std::string* v) {
      if (idx.item == SIZE_MAX || idx.loop != SIZE_MAX) {
         v = nullptr;
         return false;
      }
      else {
         assert(std::holds_alternative<::cif::Pair>(items[idx.item].data));
         v = &std::get<::cif::Pair>(items[idx.item].data).value;
         return true;
      }
   }

   bool Block::get_value(const ItemIndex& idx, std::vector<std::string>* v) {
      if (idx.item == SIZE_MAX || idx.loop == SIZE_MAX) {
         v = nullptr;
         return false;
      }
      else {
         assert(std::holds_alternative<::cif::Loop>(items[idx.item].data));
         v = &std::get<::cif::Loop>(items[idx.item].data).lpairs[idx.loop].values;
         return true;
      }
   }

   bool Block::remove_item(const std::string& t) {
      ItemIndex idx = find_tag(t);
      if (idx.item == SIZE_MAX) {
         return false;
      }
      else if (idx.loop == SIZE_MAX) { //it's a Pair
         items.erase(items.begin() + idx.item);
         return true;
      }
      else { //it's a loop
         Loop& loop = std::get<::cif::Loop>(items[idx.item].data);
         loop.lpairs.erase(loop.lpairs.begin() + idx.loop);
         return true;
      }
   }

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
      if (std::holds_alternative<Pair>(item.data))
         print(std::get<Pair>(item.data));
      else //should just be a Loop
         print(std::get<Loop>(item.data));
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
      e = std::stod(err) / std::pow(10,pow);
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

