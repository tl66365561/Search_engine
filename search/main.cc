nclude"search.h"

int main()
{
  search::Search searcher;
  bool ret = searcher.Init("../data/tmp/raw_input");
  if(!ret){
    return 1;
  }
  std::string query = "filesystem";
  std::string result;
  searcher.Searchs(query,&result);
  std::cout << "result:  " << result << std::endl;
  return 0;
}

