/**
* Searches for an A-trail from an edge code file.
* Argument 1: the edge code file.
* Argument 2 (Optional): a trail file as a sequence of edge indices. This has an extension '.trail'.
* Argument 3 (Optional): a trail file as a sequence of node indices(zero based). The trail ends with the vertice it began. The output file has extension `.ntrail'.
*/
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/config.hpp>
#include <vector>
#include <list>
#include <string>
#include <algorithm>    // std::find
#include <fstream>
#include "Atrail.hpp"



int main(int argc, char** argv)
{
	std::cout<<"INFO: Atrail_search, searchs for an Atrail for a given planar embedding."<<std::endl;
	if(argc < 2)
	{
		std::cerr << "ERROR! Usage: " << "Atrail_search " << "input_edgecode " <<"[output_edgetrail] "<<"[output_nodetrail]" <<std::endl;
		return 1;
	}else
	{
		std::string edgecode_file(argv[1]);
		std::string edgetrail_file;
		std::string nodetrail_file;
		if( argc == 2)
		{
			edgetrail_file = edgecode_file.substr(0, edgecode_file.find_last_of('.')).append(".trail");
			nodetrail_file = edgecode_file.substr(0, edgecode_file.find_last_of('.')).append(".ntrail");
		}else{
			edgetrail_file = std::string(argv[2]);
			if(argc >= 4) nodetrail_file = std::string(argv[3]);
			if(argc >= 5) std::cerr<<"WARNING: You have more arguments than required, ignoring the arguments after the third ..."<<std::endl;
		}

		std::vector<std::vector<size_t> > edge_code;

		// Attempt to read the edge code from the provided file and process if read was successful.
		if(read_edge_code(std::string(edgecode_file), edge_code))
		{
			std::list<std::size_t> edge_trail;
			Graph G(edge_code.size());
			std::size_t m = 0;
			for( unsigned int i = 0; i < edge_code.size(); ++i ) m += edge_code[i].size();
			m = m / 2;
			std::vector<std::vector<unsigned int> > ind2pair(m, std::vector<unsigned int>(0));
			for( unsigned int i = 0; i < edge_code.size(); ++i)
			{
				for( unsigned int j = 0; j < edge_code[i].size(); ++j)
				{
					ind2pair[edge_code[i][j]].push_back(i);
				}
			}

			for( unsigned int i = 0; i < m; ++i)
			{
				add_edge(ind2pair[i][0], ind2pair[i][1], i, G);
			}
			std::cout<<"INFO: Read graph from the edgecode "<<edgecode_file<<std::endl;
			//std::cout<<"INFO: Graph "<<edgecode_file<<"\n"<<to_string_graph(G)<<std::endl;
			std::list<Vertex> node_trail;
			if(Atrail_search(G, edge_code, edge_trail, node_trail) == true)
			{
				std::cout<<"INFO: Found an A-trail for the graph"<<std::endl;
				std::ofstream ofs(edgetrail_file.c_str(), std::ios::out);
				if( !ofs.is_open())
				{
					std::cerr<<"ERROR! Unable to create file "<<edgetrail_file<<std::endl;
					return 2;
				}else
				{
					std::cout<<"INFO: Appending the trail as edge list to file "<<edgetrail_file<<std::endl;
					for( std::list<std::size_t>::iterator it = edge_trail.begin(); it != edge_trail.end(); it++)
					{
							ofs<<*it<<" ";
					}
				}
				ofs.close();
				std::ofstream nfs(nodetrail_file.c_str(), std::ios::out);
				if( !nfs.is_open())
				{
					std::cerr<<"ERROR! Unable to create file "<<nodetrail_file<<std::endl;
					return 2;
				}else
				{
					std::cout<<"INFO: Appending the trail as node list to file "<<nodetrail_file<<std::endl;
					for( std::list<Vertex>::iterator it = node_trail.begin(); it != node_trail.end(); it++)
					{
							nfs<<*it<<" ";
					}
				}
				nfs.close();
				return 0;
			}else
			{
				std::cerr<<"ERROR! Unable to find an Atrail for the given code"<<std::endl;
				return 1;
			}
		} else
		{
			std::cerr<<"ERROR! Invalid edge code..., aborting with error"<<std::endl;
			return 3;
		}
	}
}

