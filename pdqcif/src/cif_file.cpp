

#include "cif_file.h"


namespace row {
   namespace cif {

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
         std::cout << p.tag << '\t' << p.value[0] << std::endl;
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
         if (item.is_pair())
            print(item.pair);
         else //should just be a Loop
            print(item.loop);
      }
      void print(const Block& block, const bool print_all_values) {
         std::cout << "---------------\nBlock:\t";
         std::cout << block.name << std::endl;
         if (print_all_values) {
            for (const Item& item : block.items) {
               std::cout << "---------------\n";
               print(item);
            }
         }
         std::cout << "---------------\n" << std::endl;
      }
      void print(const Cif& cif, const bool print_all_values) {
         std::cout << "###############\nCIF:\t";
         std::cout << cif.source << std::endl;
         for (const Block& block : cif.blocks) {
            std::cout << "###############\n";
            print(block, print_all_values);
         }
         std::cout << "###############\n" << std::endl;
      }



   }
}