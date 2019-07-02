//test for index...

#include"search.h"
int main()
{
  search::Index index;
  bool ret = index.Build("../data/tmp/raw_input");
  if(!ret){
    std::cout << "Build failed " << std::endl;
    return 1;
  }

  auto* inverted_list = index.GetInvertedList("filesystem");
  if(inverted_list == NULL){
    std::cout << "GetInvertedList failed " << std::endl;
    return 1;
  }
  for(auto weight:*inverted_list){
    std::cout << "id:  " << weight.doc_id << std::endl << "weight:" << weight.weight << std::endl;
    const auto*doc_info = index.GetDocInfo(weight.doc_id);
    std::cout << "title:  " << doc_info->title << std::endl;
    std::cout << "url:  " << doc_info->url << std::endl;
  }
  return 0;
}
