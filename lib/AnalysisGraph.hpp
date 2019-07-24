#pragma once

#include "kde.hpp"
#include <optional>
#include "kde.hpp"
#include "random_variables.hpp"

template <class T> void print(T x) { std::cout << x << std::endl; }

template <class T> void printVec(std::vector<T> xs) {
  for (auto x : xs) {
    print(x);
  }
}


struct Event {
  std::string adjective;
  int polarity;
  std::string concept_name;

  Event( std::string adj, int pol, std::string con_name )
    : adjective{ adj }, polarity{ pol }, concept_name{ con_name }
  {
  }

  Event( std::tuple< string, int, string > evnt )
  {
    adjective = std::get< 0 >( evnt );
    polarity = std::get< 1 >( evnt );
    concept_name = std::get< 2 >( evnt );
  }
};


struct Statement {
  Event subject;
  Event object;
};


/*
struct CausalFragment {
  std::string subj_adjective;
  std::string obj_adjective;
  // Here we assume that unknown polarities are set to 1.
  int subj_polarity{1};
  int obj_polarity{1};
};
*/


struct Edge {
  std::string name;
  // TODO: Why kde is optional?
  // According to AnalysisGraph::construct_beta_pdfs()
  // it seems all the edges have a kde
  std::optional<KDE> kde;
  //std::vector<CausalFragment> causalFragments = {};

  std::vector< Statement > evidence;

  // The current β for this edge
  // TODO: Need to decide how to initialize this or
  // decide whethr this is the correct way to do this.
  double beta = 1.0;
};

struct Node {
  std::string name = "";
  bool visited;
  LatentVar rv;

  std::vector< Indicator > indicators;
  // Maps each indicator name to its index in the indicators vector
  std::map< std::string, int > indicator_names;

  void add_indicator( string indicator, string source )
  {
    //TODO: What if this indicator already exists?
    //      At the moment only the last indicator is recorded
    //      in the indicator_names map
    //What if this indicator already exists?
    //*Loren: We just say it's already attached and do nothing.
    // As of right now, we are only attaching one indicator per node but even
    // if we were attaching multiple indicators to one node, I can't yet think
    // of a case where the numerical id (i.e. the order) matters. If we do come
    // across that case, we will just write a function that swaps ids.*
    if( indicator_names.find( indicator ) != indicator_names.end() )
    {
      std::cout << indicator << " already attached to " << name << std::endl;
      return;
    }

    indicator_names[ indicator ] = indicators.size();
    indicators.push_back( Indicator( indicator, source ));
  }


  void replace_indicator( string indicator_old, string indicator_new, string source )
  {
    auto map_entry =  indicator_names.extract( indicator_old );

    if( map_entry ) // indicator_old is in the map
    {
      // Update the map entry and add the new indicator
      // in place of the old indicator
      map_entry.key() = indicator_new;
      indicator_names.insert( move( map_entry ));
      indicators[ map_entry.mapped() ] = Indicator( indicator_new, source );
    }
    else // indicator_old is not attached to this node
    {
      std::cout << "Node::replace_indicator - indicator " << indicator_old << " is not attached to node " << name << std::endl;
      std::cout << "\tAdding indicator " << indicator_new << " afresh\n";
      add_indicator( indicator_new, source );
    }
  }
  

  template <class T> void set_indicator_attribute( string indicator, string attribute, T value )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::set_indicator_attribute()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\tattribute: " << attribute << " cannot be set" << std::endl;
    }
    switch(attribute) {
      case "source" : indicators[ind_index].source = value;
                      break;
      case "unit"   : indicators[ind_index].unit = value;
                      break;
      case "mean"   : indicators[ind_index].mean = value;
                      break;
      case "value"  : indicators[ind_index].value = value;
                      break;
      case "stdev"  : indicators[ind_index].stdev = value;
                      break;
      case "time"   : indicators[ind_index].time = value;
                      break;
      case "aggaxes": indicators[ind_index].aggaxes = value;
                      break;
      case "aggregation_method": indicators[ind_index].aggregation_method = value;
                                 break;
      case "timeseries": indicators[ind_index].timeseries = value;
                         break;
      case "samples": indicators[ind_index].samples = value;
                      break;
      case "name"   : std::cout << "Cannot change indicator name, use Node::replace_indcator() instead." << std::endl;
                      break;
      default       : std::cerr << "Error: Node::set_indicator_attribute()\n"
                               << "attribute: " << attribute << " does not exist" << std::endl;
 
    }
  }


  template <class T> T get_indicator_attribute( string indicator, string attribute)
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::get_indicator_attribute()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\tattribute: " << attribute << " cannot be retrieved" << std::endl;
    }
    T value;
    switch(attribute) {
      case "source" : value = indicators[ind_index].source;
                      break;
      case "unit"   : value = indicators[ind_index].unit;
                      break;
      case "mean"   : value = indicators[ind_index].mean;
                      break;
      case "value"  : value = indicators[ind_index].value;
                      break;
      case "stdev"  : value = indicators[ind_index].stdev;
                      break;
      case "time"   : value = indicators[ind_index].time;
                      break;
      case "aggaxes": value = indicators[ind_index].aggaxes;
                      break;
      case "aggregation_method": value = indicators[ind_index].aggregation_method;
                                 break;
      case "timeseries": value = indicators[ind_index].timeseries;
                         break;
      case "samples": value = indicators[ind_index].samples;
                      break;
      case "name"   : value = indicators[ind_index].name;
                      break;
      default       : std::cerr << "Error: Node::get_indicator_attribute()\n"
                               << "attribute: " << attribute << " does not exist" << std::endl;
 
    }
  }




  /*void set_indicator_source( string indicator, string source )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::set_indicator_source()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\tsource: " << source << " cannot be set" << std::endl;
    }
    indicators[ind_index].source = source;
  }


  string get_indicator_source( string indicator )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::get_indicator_source()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\tsource could not be retrieved" << std::endl;
    }
    return indicators[ind_index].source;
  }


  void set_indicator_unit( string indicator, string unit )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::set_indicator_unit()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\tunit: " << unit << " cannot be set" << std::endl;
    }
    indicators[ind_index].unit = unit;
  }


  string get_indicator_unit( string indicator )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::get_indicator_unit()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\tunit could not be retrieved" << std::endl;
    }
    return indicators[ind_index].unit;
  }


  void set_indicator_mean( string indicator, double mean )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::set_indicator_mean()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\tmean: " << mean << " cannot be set" << std::endl;
    }
    indicators[ind_index].mean = mean;
  }


  double get_indicator_mean( string indicator )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::get_indicator_mean()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\tmean could not be retrieved" << std::endl;
    }
    return indicators[ind_index].mean;
  }


  void set_indicator_value( string indicator, double value )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::set_indicator_value()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\tvalue: " << value << " cannot be set" << std::endl;
    }
    indicators[ind_index].value = value;
  }


  double get_indicator_value( string indicator )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::get_indicator_value()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\tvalue could not be retrieved" << std::endl;
    }
    return indicators[ind_index].value;
  }


  void set_indicator_stdev( string indicator, double stdev )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::set_indicator_stdev()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\tstdev: " << stdev << " cannot be set" << std::endl;
    }
    indicators[ind_index].stdev = stdev;
  }


  double get_indicator_stdev( string indicator )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::get_indicator_stdev()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\tstdev could not be retrieved" << std::endl;
    }
    return indicators[ind_index].stdev;
  }

  //uses temporary time type
  void set_indicator_time( string indicator, string time )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::set_indicator_time()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\ttime: " << time << " cannot be set" << std::endl;
    }
    indicators[ind_index].time = time;
  }

  //uses temporary time type
  string get_indicator_time( string indicator )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::get_indicator_time()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\ttime could not be retrieved" << std::endl;
    }
    return indicators[ind_index].time;
  }


  void set_indicator_aggaxes( string indicator, vector< std::string > aggaxes )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::set_indicator_aggaxes()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\taggaxes: given aggaxes cannot be set" << std::endl;
    }
    indicators[ind_index].aggaxes = aggaxes;
  }


  vector< std::string > get_indicator_aggaxes( string indicator )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::get_indicator_aggaxes()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\taggaxes could not be retrieved" << std::endl;
    }
    return indicators[ind_index].aggaxes;
  }


  void set_indicator_aggregation_method( string indicator, string aggregation_method )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::set_indicator_aggregation_method()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\taggregation_method: " << aggregation_method << " cannot be set" << std::endl;
    }
    indicators[ind_index].aggregation_method = aggregation_method;
  }


  string get_indicator_aggregation_method( string indicator )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::get_indicator_aggregation_method()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\taggregation_method could not be retrieved" << std::endl;
    }
    return indicators[ind_index].aggregation_method;
  }


  void set_indicator_timeseries( string indicator, double timeseries )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::set_indicator_timeseries()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\ttimeseries: " << timeseries << " cannot be set" << std::endl;
    }
    indicators[ind_index].timeseries = timeseries;
  }


  double get_indicator_timeseries( string indicator )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::get_indicator_timeseries()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\ttimeseries could not be retrieved" << std::endl;
    }
    return indicators[ind_index].timeseries;
  }


  void set_indicator_samples( string indicator, vector< double > samples )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::set_indicator_samples()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\tsamples: given samples cannot be set" << std::endl;
    }
    indicators[ind_index].samples = samples;
  }


  vector< double > get_indicator_samples( string indicator )
  {
    int ind_index;
    try {
      ind_index = indicator_names[indicator];
    } catch (const std::out_of_range &oor) {
      std::cerr << "Error: Node::get_indicator_samples()\n"
                << "\tIndicator: " << indicator << " does not exist\n";
      std::cerr << "\tsamples could not be retrieved" << std::endl;
    }
    return indicators[ind_index].samples;
  }
*/


  
  //Utility function that clears the indicators vector and the name map.
  void clear_indicators()
  {
    indicators.clear();
    indicator_names.clear();
  }
};

struct GraphData {
  std::string name;
};

typedef boost::adjacency_list<boost::setS,
                              boost::vecS,
                              boost::bidirectionalS,
                              Node,
                              Edge,
                              GraphData>
    DiGraph;
