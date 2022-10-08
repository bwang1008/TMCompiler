/**
 * Given a graph and a start node, find shortest distance from start node to all other nodes. -1 if not reachable
 * Input: First line is 3 integers: N M S, representing number of nodes,
 * number of edges, and the start node (1 <= S <= N)
 * Then the following M lines each contain two integers, u v (1 <= u,v <= N),
 * representing an edge between nodes u and v.
 * Output the shortest distances from S to all N nodes, -1 if not reachable
 */
int main() {
	int numNodes = nextInt();
	int numEdges = nextInt();
	int startNode = nextInt() - 1;

	// next M lines are edges between nodes
	int temp = 0;
	for(int i = 0; i < numEdges; i += 1) {
		MEM[2 * i] = nextInt() - 1;
		MEM[2 * i + 1] = nextInt() - 1;
	}

	// memory layout: first 2 * numEdges is reserved for edges
	// then next (numNodes) integers is for distance from start node to other nodes
	// then reserver a queue afterwards
	
	// first set distances to all nodes from start as "infinity": aka just numNodes
	for(int i = 0; i < numNodes; i += 1) {
		MEM[2 * numEdges + i] = numNodes;
	}

	MEM[2 * numEdges + startNode] = 0;

	// define queue	
	int queueHead = 2 * numEdges + numNodes;
	int queueTail = queueHead + 1;

	// place startNode in queue
	MEM[queueHead] = startNode;

	while(queueHead != queueTail) {
		int node = MEM[queueHead];
		queueHead += 1;

		// find all neighbors of node
		for(int i = 0; i < numEdges; i += 1) {
			int u = MEM[2 * i];
			int v = MEM[2 * i + 1];

			int neighbor = -1;

			if(u == node) {
				neighbor = v;
			}
			else if(v == node) {
				neighbor = u;
			}

			if(neighbor != -1) {
				// handle only if haven't visited yet: aka if MEM[neighbor] == numNodes
				if(MEM[2 * numEdges + neighbor] == numNodes) {
					// if haven't visited neighbor, then neighbor distance is 1 more than node's distance
					MEM[2 * numEdges + neighbor] = 1 + MEM[2 * numEdges + node];
					// push to queue
					MEM[queueTail] = neighbor;
					queueTail += 1;
				}
			}
		}
	}

	// the unreachable nodes are the ones still with dist == numNodes
	for(int i = 0; i < numNodes; i += 1) {
		int dist = MEM[2 * numEdges + i];

		if(dist == numNodes) {
			printInt(-1);
			printSpace();
		}
		else {
			printInt(dist);
			printSpace();
		}
	}

	return 0;
}
/*
5 3 1
1 2
1 3
3 4

Which in bits is 0101_011_01_01_001_01_011_011_0001
Desired output: 0 1 1 2 -1 (which in bits would be 0_01_01_001_11)
*/
