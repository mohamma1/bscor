/*
* postman_tour.cpp
*
*  Created on: Jan 15, 2015
*  Author: Abdulmelik Mohammed
*  Creates a multigraph from a simple graph such that the graph is Eulerian
*  Argument 1: filename of input graph in dimacs format.
*  Argument 2: filename of output multigraph in dimacs format.
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include "boost_graph_helper.hpp"
#include <lemon/list_graph.h>
#include <lemon/matching.h>
#include <lemon/concepts/graph.h>
#include <lemon/full_graph.h>

#define NDEBUG
#ifdef DEBUG
#define DEBUGPRINT( x ) x
#else
#define DEBUGPRINT( x )
#endif

using namespace lemon;
using namespace std;

typedef FullGraph::EdgeMap<signed int> Distances;
typedef MaxWeightedPerfectMatching<FullGraph, Distances> MWPM;

bool is_even(Graph G);

int main(int argc, char* argv[])
{
	std::cout << "INFO: postman_tour, creates a multigraph from a simple graph using a min weight perfect matching to make the graph Eulerian. " << std::endl;
	if (argc != 3)
		std::cerr << "ERROR! Usage: postman_tour input_dimacs output_dimacs" << std::endl;
	else
	{
		std::string infile(argv[1]);
		std::string outfile(argv[2]);
		Graph ingraph;
		if (!read_dimacs(infile, ingraph)) return 1;

		std::vector<int> component(num_vertices(ingraph));
		unsigned int num_components = connected_components(ingraph, &component[0]);
		assert(num_components == 1);
		if (num_components != 1)
		{
			std::cerr << "ERROR! The graph of the ply file is not connected. Exiting with error!" << std::endl;
			return 1;
		}
		VertexIterator vit, vend;
		std::vector<Vertex> old_ind(0);
		std::size_t n = num_vertices(ingraph);
		std::size_t num_odd_vertices = 0;
		property_map < Graph, edge_weight_t >::type w = boost::get(edge_weight, ingraph);
		EdgeIterator e_it, e_end;
		boost::tie(e_it, e_end) = edges(ingraph);
		for (; e_it != e_end; e_it++)
		{
			w[*e_it] = 1;
		}
		std::cout << "INFO: Finding odd degree vertices ..." << std::endl;
		for (boost::tie(vit, vend) = vertices(ingraph); vit != vend; ++vit)
		{
			if ((degree(*vit, ingraph) % 2) == 1)
			{
				old_ind.push_back(*vit);
				num_odd_vertices++;
			}
		}

		Graph multigraph = ingraph;
		if (num_odd_vertices > 0)
		{
			Graph oddgraph(num_odd_vertices);
			std::vector<std::vector<int> > D(n, std::vector<int>(n, 0));
			get(edge_weight, ingraph);
			boost::tie(e_it, e_end) = edges(ingraph);

			std::cout << "INFO: Finding the shortest paths between odd degree vertices ..." << std::endl;
			johnson_all_pairs_shortest_paths(ingraph, D);

			// Add edges to the complete graph
			std::size_t e_ind = 0;
			std::cout << "INFO: Constructing a complete graph from the odd degree vertices ..." << std::endl;
			for (long unsigned int i = 0; i < num_odd_vertices; ++i)
			{
				for (long unsigned int j = i + 1; j < num_odd_vertices; ++j)
				{
					Edge e = boost::add_edge(i, j, e_ind, oddgraph).first;
					boost::put(edge_weight, oddgraph, e, D[old_ind[i]][old_ind[j]]);
					e_ind++;
				}
			}

			FullGraph lg = FullGraph(num_odd_vertices); // lemon complete graph for min weight perfect matching
			Distances emap = Distances(lg, 1); // Unspecified distances set to 1
			EdgeIterator e_it, e_end;
			boost::tie(e_it, e_end) = edges(oddgraph);
			// Copying edge weights from boost graph to lemon full graph

			for (; e_it != e_end; e_it++)
			{
				//std::cout << "e " << e_it->m_source << " " << e_it->m_target << " " << get(edge_weight, ingraph, *e_it)<<std::endl;
				emap[lg.edge(lg(e_it->m_source), lg(e_it->m_target))] = -1 * get(edge_weight, ingraph, *e_it);
			}
			// Max weight perfect matching on lemon graph == min weight matching on orignal dimacs graph
			MWPM mwpm = MWPM(lg, emap);
			std::cout << "INFO: Running the min weight matching algorithm ..." << std::endl;
			mwpm.run();

			std::vector< std::pair<Vertex, Vertex> > match;
			std::vector<bool> added = std::vector<bool>(num_odd_vertices, false);
			for (size_t i = 0; i < num_odd_vertices; ++i)
			{
				if (added[i] == false)
				{
					match.push_back(std::make_pair(i, lg.index(mwpm.mate(lg(i)))));
					added[i] = true;
					added[lg.index(mwpm.mate(lg(i)))] = true;
				}
			}
			
			boost::tie(e_it, e_end) = edges(ingraph);
			std::vector<std::size_t> parallel_edge_count(n*n, 0);
			std::size_t u, v, tmp;
			DEBUGPRINT(to_string_graph(ingraph);)
			for (; e_it != e_end; e_it++)
			{
				u = (std::size_t) source(*e_it, ingraph);
				v = (std::size_t) target(*e_it, ingraph);
				if (u > v) { tmp = u; u = v; v = tmp; }
				//std::cout<<"pec[" <<u * (n-1) - ((u * (u-1)) / 2) + v - u<<"]";
				(parallel_edge_count[u * (n - 1) - ((u * (u - 1)) / 2) + v - u - 1])++; // edges ordered row by row -> index = (sum r=0 to r=u-1 (n-1-r)) + v - u
				w[*e_it] = 1;
			}

			//std::cout<<"INFO: Graph before addition of multiedges"<<std::endl;
			DEBUGPRINT(std::cout << to_string_graph(ingraph);)

			std::vector<Vertex> p(n);

			// Adding multiedges along the path of each matched pair to make all vertices have even degree.
			std::cout << "INFO: Adding edges along the shortest paths ..." << std::endl;

			//unsigned int num_components = connected_components(ingraph, &component[0]);
			for (std::vector<std::pair<Vertex, Vertex> >::iterator vit = match.begin(); vit != match.end(); ++vit)
			{
				DEBUGPRINT(std::cout << "INFO: Adding multiedges along path from " << old_ind[vit->first] << " to " << old_ind[vit->second] << " ..." << std::endl;)
					//assert(component[old_ind[vit->first]] == component[old_ind[vit->second]]);
				dijkstra_shortest_paths(ingraph, old_ind[vit->first],
					predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, ingraph))));

				Vertex current = old_ind[vit->second];
				while (current != old_ind[vit->first])
				{
					DEBUGPRINT(std::cout << current << " ";)
						u = ((current < p[current]) ? current : p[current]);
					v = ((current > p[current]) ? current : p[current]);
					if (parallel_edge_count[u*(n - 1) - ((u*(u - 1)) / 2) + v - u - 1] == 2) 
					{
						remove_edge(current, p[current], multigraph); // with the new edge to add, the older two can be removed
						parallel_edge_count[u*(n - 1) - ((u*(u - 1)) / 2) + v - u - 1] = 0;
					}
					add_edge(current, p[current], multigraph);
					parallel_edge_count[u*(n - 1) - ((u*(u - 1)) / 2) + v - u - 1]++;
					current = p[current];
				}

				DEBUGPRINT(std::cout << current << std::endl;)
			}
			DEBUGPRINT(std::cout << "INFO: Graph after addition of multiedges" << std::endl;)

		}
		else
		{
			std::cout << "INFO: There were no odd degree vertices!" << std::endl;
		}
		DEBUGPRINT(std::cout << to_string_graph(multigraph);)
		assert(is_even(multigraph));
		if (!is_even(multigraph))
		{
			std::cerr << "ERROR! an unknown error occurred. Output file not written." << std::endl;
			return 1;
		}
		write_dimacs(outfile, multigraph);
		std::cout << "INFO: Wrote an Eulerian multigraph after addition of multiedges to " << outfile << std::endl;
		return EXIT_SUCCESS;

	}
	return EXIT_SUCCESS;
}

bool is_even(Graph G)
{
	bool even = true;
	std::size_t n = num_vertices(G);
	for (std::size_t i = 0; i < n; i++)
	{
		if (degree(i, G) % 2 == 1)
		{
			even = false;
			break;
		}
	}
	return even;
}


