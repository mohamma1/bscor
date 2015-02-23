/*
 * embed_planar.cpp
 *
 *  Created on: Jul 18, 2013
 *  Author: Abdulmelik Mohammed
 *  Embeds the graph of the object to the plane so that an A-trail can be obtained from the embedding
 *  Argument 1: name of the graph
 *  Argument 2 (Optional): embedding as an edge code. 
 *  An edge code is the file which gives for each vertex, the cyclic order of its incident edges according to the embedding. The original graph can be obtained 
 *  from this file since any edge of the graph can be decoded from the two vertices where the edge is in their order. The edge code has an extension '.ecode'.
 */

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include "boost_graph_helper.hpp"

#define NDEBUG
#ifdef DEBUG
#define DEBUGPRINT( x ) x
#else
#define DEBUGPRINT( x )
#endif

int main(int argc, char* argv[])
{
	std::cout<<"INFO: embed_planar, outputs a planar embedding of a graph as an edge ordering for each vertex."<<std::endl;
	if( argc < 2 )
	{
		std::cerr<<"ERROR! usage: embed_planar input_dimacs_file [output_edgecode_file]"<<std::endl;
		return 1;
	}else
	{
		std::string inputfile(argv[1]);
		std::string outputfile;
		if( argc >= 3)
		{
			if( argc > 3) std::cerr<<"WARNING! You have more arguments than required, ignoring the arguments after the second! Type embed_planar for usage instructions."<<std::endl;
			outputfile = std::string(argv[2]);
		}else outputfile = inputfile.substr(0, inputfile.find_last_of('.')).append(".ecode");

		Graph G;
		if(!read_dimacs(inputfile, G)) return 1;
		planar_embedding_storage_t planar_embedding_storage(num_vertices(G));
		planar_embedding_t planar_embedding(planar_embedding_storage.begin(),
					get(vertex_index, G)
			);
		if(!boost::boyer_myrvold_planarity_test(G, planar_embedding))
		{
			std::cerr<<"ERROR! The graph is not planar, exiting with error!" <<std::endl;
			return 1;
		}else
		{
			std::cout<<"INFO: The graph is planar."<<std::endl;
		}
		std::vector<std::vector<std::size_t> > edge_code;
		embedding_to_edge_code(planar_embedding, G, edge_code);
		write_edge_code(outputfile, edge_code);
		std::cout<<"INFO: Wrote the embedding to "<<outputfile<<std::endl;
		return EXIT_SUCCESS;
	}

}
