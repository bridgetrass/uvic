/*  
   Bridget Rassell V00804180
   ShortestPath.java
   
   
   
   CSC 226 - Fall 2014
   Assignment 3 - Template for Dijkstra's Algorithm
   
   This template includes some testing code to help verify the implementation.
   To interactively provide test inputs, run the program with
	java ShortestPath
	
   To conveniently test the algorithm with a large input, create a text file
   containing one or more test graphs (in the format described below) and run
   the program with
	java ShortestPath file.txt
   where file.txt is replaced by the name of the text file.
   
   The input consists of a series of graphs in the following format:
   
    <number of vertices>
	<adjacency matrix row 1>
	...
	<adjacency matrix row n>
	
   Entry A[i][j] of the adjacency matrix gives the weight of the edge from 
   vertex i to vertex j (if A[i][j] is 0, then the edge does not exist).
   Note that since the graph is undirected, it is assumed that A[i][j]
   is always equal to A[j][i].
	
   An input file can contain an unlimited number of graphs; each will be 
   processed separately.


   B. Bird - 08/02/2014
*/

import java.util.Arrays;
import java.util.Scanner;
import java.util.Vector;
import java.io.File;

// import list to store neighbours of vertices
import java.util.LinkedList;

//Do not change the name of the ShortestPath class
public class ShortestPath{

	/* ShortestPath(G)
		Given an adjacency matrix for graph G, return the total weight
		of a minimum weight path from vertex 0 to vertex 1.
		
		If G[i][j] == 0, there is no edge between vertex i and vertex j
		If G[i][j] > 0, there is an edge between vertices i and j, and the
		value of G[i][j] gives the weight of the edge.
		No entries of G will be negative.
	*/
	static int ShortestPath(int[][] G){
		int numVerts = G.length;
		int totalWeight = 0;
		int[] prev = new int[numVerts];
        int[] dist = new int[numVerts];

		
        HeapPriorityQueue Q = new HeapPriorityQueue(numVerts);
        
        // for get vertex get neighbour list
        LinkedList<LinkedList<Integer>> adj = new LinkedList<LinkedList<Integer>>();
		
        for (int i = 0; i < numVerts; i++) {
            LinkedList<Integer> friends = new LinkedList<Integer>();
            for (int j = 0; j < numVerts; j++) {
                if (G[i][j] > 0) {
                    friends.add(j);
                }
            }
            // add vertices next to vertex to adj list
            adj.add(friends);
            
            if (i == 0) {
                dist[i] = 0;  
                prev[0] = -1;
                Vertex v = new Vertex(0,0);
                v.adj = friends;
                Q.insert(v);
            }
            else {
                dist[i] = -1;
                prev[i] = -1;  
            }
        }
        
        while(!Q.isEmpty()) {
            Vertex u = Q.removeMin();
            int i = u.index;
            if (i == 1) {
                return u.dist;
            }
            
            //each adj of u
            int friends = u.adj.size();
            for (int k = 0; k < friends; k++) {
                int j = u.adj.poll();
                int alt = dist[i] + G[i][j];
                if (alt < dist[j] || dist[j] == -1) {
                    dist[j] = alt;
                    prev[j] = i;
                    Vertex v = new Vertex(j,alt);
                    v.adj = adj.get(j);
                    //insert it
                    if (!Q.swap(v)) {
                        Q.insert(v);
                    }
                }
            }
        }
		return totalWeight;
		
	}
	
    private static class Vertex implements Comparable<Vertex> {
		public LinkedList<Integer> adj;
        public int index;
        public Integer dist;

        
        public Vertex(int u, Integer l) {
            index = u;
            dist = l;
            adj = new LinkedList<Integer>();
        }
        
        // check verts equal
        public boolean equals(Object o) {
            if (index == ((Vertex)o).index) {
                return true;
            }
            return false;
        }
         
        //Compares two vertices by shortest path from source 
        public int compareTo(Vertex u) {
            return dist.compareTo(u.dist);
        }
    }
	
    // Heap for insert remove swap swap insert
    private static class HeapPriorityQueue {
		private int currentSize;
        private Vertex storage[];
		
        public HeapPriorityQueue(int size) {
            storage = new Vertex[size + 1];
            currentSize = 0;
        }
   
        public int size () {
            return currentSize;
        }
        
        public boolean isEmpty() {
            return size() == 0;
        }
        
        public Vertex removeMin() {
            if (isEmpty()) return null;
            Vertex value = storage[1];
            swapElement(1, currentSize);
            storage[currentSize] = null;
            currentSize--;
            bubbleDown(1);
            return value;
        }

        public void insert(Vertex v) {
            if ((currentSize + 1) == storage.length) return;
			
            storage[currentSize + 1] = v;
			
            currentSize++;
			
            bubbleUp(currentSize);
        }
        
        // Swaps with orig copy
        public boolean swap(Vertex v) {
            for (int i = 1; i <= currentSize; i++) {
                if (storage[i].equals(v)) {
                    // swap vertices
                    storage[i] = v;
                    // swapped vertex min of heap
                    if (parent(i) == 0) {
                        bubbleDown(1);
                        return true;
                    }
                    // has children
                    else if (hasLeft(i)) {
                        // if parent is < node
                        storage[i] = v;
                        if (v.compareTo(storage[parent(i)]) <= 0) {
                            bubbleUp(i);
                            return true;
                        }
                        // if vertex is > children
                        int pos = 0;
                        // vertex has 2 children
                        if (hasLeft(i) && hasRight(i)) {
                            // find which child is min
                            if (storage[leftChild(i)].compareTo(storage[rightChild(i)]) <= 0) {
                                //left child is less than right child
                                pos = leftChild(i);
                            } else {
                                // right child is less than left child
                                pos = rightChild(i);
                            }
                            // compare vertex to min child to bubbleDown
                            if (v.compareTo(storage[pos]) > 0) {
                                bubbleDown(i);
                                return true;
                            } else {
                                break;
                            }
                        }
                        // vertex only has right child
                        else if (hasRight(i)) {
                            if (v.compareTo(storage[rightChild(i)]) > 0) {
                                bubbleDown(i);
                                return true;
                            } else {
                                break;
                            }
                        }
                        // else vertex only has left child
                        else {
                            if (v.compareTo(storage[leftChild(i)]) > 0) {
                                bubbleDown(i);
                                return true;
                            } else {
                                break;
                            }
                        }
                    }
                    // no children
                    else {
                        bubbleUp(i);
                        return true;
                    }
                }
            }
            return false;
        }

        private void bubbleUp(int i) {
            boolean stop = false;
            while (parent(i) != 0 && !stop) {
                int pos = parent(i);
                int result = storage[i].compareTo(storage[pos]);
                if (result < 0) {
                    swapElement(pos, i);
                    i = pos;
                } else {
                    stop = true;
                }
            }
        }
        
        private void bubbleDown(int i) {
            boolean done = false;
            // Check index has a left child
            boolean checkLeft = hasLeft(i);
            // iterate down left child
            while (checkLeft && !done) {
                // get index of the leftChild
                int left = leftChild(i);
                int result;
                // check if index has a right child
                if (hasRight(i)) {
                    //index of the rightChild
                    int right = rightChild(i);
                    // Compare left and right child
                    result = storage[left].compareTo(storage[right]);
                    // If right is less than left check right with current index
                    if (result > 0) {
                        //right is less than left don't check left
                        checkLeft = false;
                        // Compare the right child to the current index
                        result = storage[i].compareTo(storage[right]);
                        //right element is larger than current index swap
                        if (result > 0) {
                            swapElement(i, right);
                            i = right;
                        } else {
                            done = true;
                        }
                    }
                }
                // Check the left child vs the current index
                if (checkLeft) {
                    left = leftChild(i);
                    result = storage[i].compareTo(storage[left]);
                    // If the left child is less than the current index swap
                    if (result > 0) {
                        swapElement(i, left);
                        i = left;
                    } else {
                        done = true;
                    } 
                }
                
                checkLeft = hasLeft(i);
            }
        }
        private void swapElement(int p1, int p2) {
            Vertex v = storage[p1];
            storage[p1] = storage[p2];
            storage[p2] = v;
        }
		
        private boolean hasLeft(int pos) {
            return pos*2 <= currentSize;
        }
        
        private boolean hasRight (int pos) {
            return (pos*2 + 1) <= currentSize;
        }
		
        private int leftChild(int pos) {
            return pos*2;
        }

        private int rightChild(int pos) {
            return pos*2 + 1;
        }
		
        private int parent(int pos) {
            return pos/2;
        }

		
		
		
    }
	
	
	
	
	
	
	/* main()
	   Contains code to test the ShortestPath function. You may modify the
	   testing code if needed, but nothing in this function will be considered
	   during marking, and the testing process used for marking will not
	   execute any of the code below.
	*/
	public static void main(String[] args){
		Scanner s;
		if (args.length > 0){
			try{
				s = new Scanner(new File(args[0]));
			} catch(java.io.FileNotFoundException e){
				System.out.printf("Unable to open %s\n",args[0]);
				return;
			}
			System.out.printf("Reading input values from %s.\n",args[0]);
		}else{
			s = new Scanner(System.in);
			System.out.printf("Reading input values from stdin.\n");
		}
		
		int graphNum = 0;
		double totalTimeSeconds = 0;
		
		//Read graphs until EOF is encountered (or an error occurs)
		while(true){
			graphNum++;
			if(graphNum != 1 && !s.hasNextInt())
				break;
			System.out.printf("Reading graph %d\n",graphNum);
			int n = s.nextInt();
			int[][] G = new int[n][n];
			int valuesRead = 0;
			for (int i = 0; i < n && s.hasNextInt(); i++){
				for (int j = 0; j < n && s.hasNextInt(); j++){
					G[i][j] = s.nextInt();
					valuesRead++;
				}
			}
			if (valuesRead < n*n){
				System.out.printf("Adjacency matrix for graph %d contains too few values.\n",graphNum);
				break;
			}
			long startTime = System.currentTimeMillis();
			
			int totalWeight = ShortestPath(G);
			long endTime = System.currentTimeMillis();
			totalTimeSeconds += (endTime-startTime)/1000.0;
			
			System.out.printf("Graph %d: Minimum weight of a 0-1 path is %d\n",graphNum,totalWeight);
		}
		graphNum--;
		System.out.printf("Processed %d graph%s.\nAverage Time (seconds): %.2f\n",graphNum,(graphNum != 1)?"s":"",(graphNum>0)?totalTimeSeconds/graphNum:0);
	}
}