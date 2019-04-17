#include"../common/util.hpp"

namespace searcher{

  struct DocInfo{

   uint64_t doc_id;
   std::string title;
   std::string content;
   std::string url;
}
  

  struct weight{
   
    uint64_t doc_id;
    int weight;
  }



typedef vector<weight> inverted_list;


  class index{




   private:
     std::vector<DocInfo>_forward_index;
     std::unordermap<std::string,inverted_list>_inverted_index;
     cppjieba::Jieba jieba_;
   public:
     bool build(std::string input_path)const;
     DocInfo* GetDocInfo(uint64_t Doc_id);
     inverted_list* GetInverted_List(std::string key);

   private:
       Docinfo*  build_forward_index(const std::string& line);
       void build_inverted_index(const DocInfo& doc );

  }















}//end searcher;




