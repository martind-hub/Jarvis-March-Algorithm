
#ifndef ___READ_POINTS_H___
#define ___READ_POINTS_H___


#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>

template<typename data_type,
	 template <typename... table_type_args> typename table_type,
	 template <typename... row_type_args> typename row_type>
table_type<row_type<data_type> > csvtable(const std::string& filename)
{
  using boost::lexical_cast;
  table_type<row_type<data_type> > table;
  std::ifstream infile(filename);
  while(infile)
    {
      std::string s;
      if(!getline(infile,s)) break;
      std::istringstream ss(s);
      row_type<data_type> row;
      while(ss)
	{
	  std::string s;
	  if(!getline(ss,s,',')) break;
	  row.push_back(lexical_cast<data_type>(s)); 
	}
      table.push_back(row);
    }
  return table;
}

#include <list>
#include <vector>

#define readpoints csvtable<double,std::list,std::vector>


#endif


