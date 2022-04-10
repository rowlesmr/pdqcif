#include "cif.h"

namespace row {
   namespace cif {

      std::string check_duplicates(const Block& block) {
         std::unordered_map<std::string, size_t> tagCount;

         std::string tag{};
         //collect all the tags in the entire block.
         for (const Item& item : block.items) {
            if (item.type == ItemType::Pair) {
               tag = item.pair.tag;

               if (tagCount.find(tag) == tagCount.end()) {
                  tagCount.insert(std::make_pair(tag, 1));
               }
               else {
                  tagCount[tag]++;
               }
            }
            else if (item.type == ItemType::Loop) {
               for (const LoopPair& lp : item.loop.lpairs) {
                  tag = lp.tag;
                  if (tagCount.find(tag) == tagCount.end()) {
                     tagCount.insert(std::make_pair(tag, 1));
                  }
                  else {
                     tagCount[tag]++;
                  }
               }
            }
         }

         std::string duplicates{};
         for (const auto& [key, value] : tagCount) {
            if (value > 1)
               duplicates += key + " ";
         }
         return duplicates;
      }

      void check_duplicates(const Cif& cif) {
         std::string duplicates{};
         std::string blockDuplicates{};
         for (const Block& block : cif.blocks) {
            blockDuplicates = check_duplicates(block);
            if (blockDuplicates.size() > 0) {
               duplicates += block.name + ": " + blockDuplicates + "\n";
            }
         }
         if (duplicates.size() > 0) {
            throw std::runtime_error("Duplicate tags encountered: " + duplicates);
         }
      }

      //read in a file into a Cif. Will throw std::runtime_error if it encounters problems
      Cif read_file(const std::string& filename) {
         pegtl::file_input in(filename);
         return read_input(in);
      }

   }
}