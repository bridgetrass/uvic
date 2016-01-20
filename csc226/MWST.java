/*
	Bridget Rassell V00804180






   MWST.java
   CSC 226 - Spring 2015
   Assignment 1 - Minimum Weight Spanning Tree Template
   
   This template includes some testing code to help verify the implementation.
   To interactively provide test inputs, run the program with
	java MWST
	
   To conveniently test the algorithm with a large input, create a text file
   containing one or more test graphs (in the format described below) and run
   the program with
	java MWST file.txt
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
*/

import java.util.Arrays;
import java.util.Scanner;
import java.util.Vector;
import java.io.File;
import java.util.Comparator;
import java.util.*;

//Do not change the name of the MWST class
public class MWST{

	/* mwst(G)
		Given an adjacency matrix for graph G, return the total weight
		of all edges in a minimum weight spanning tree.
		
		If G[i][j] == 0, there is no edge between vertex i and vertex j
		If G[i][j] > 0, there is an edge between vertices i and j, and the
		value of G[i][j] gives the weight of the edge.
		No entries of G will be negative.
	*/
	static int MWST(int[][] G){
		int totalWeight = 0;
		int numVerts = G.length;
		PriorityQueue<Edge> EdgeQ = new PriorityQueue<Edge>();
		LinkedList<Edge> MWST = new LinkedList<Edge>();
		
		for(int i= 0; i<=numVerts;i++){
			for (int j=i;j<numVerts; j++){ 
				if(G[i][j]!=0){
					Edge e = new Edge(i,j, G[i][j]);
					EdgeQ.add(e);
				}
			}
		}
		UF myUF = new UF(numVerts);
		while(EdgeQ.size()>0&&MWST.size()< numVerts-1 ){
			
			Edge e = EdgeQ.remove();
			int v = e.either();
			int w = e.other(v);
			if (!myUF.connected(v, w)){
				myUF.union(v,w);
				MWST.add(e);
				totalWeight+=e.weight();
				
			}

		
		}
		
		
		
		// build PQ, delete min, union - find, connected

		/* Find a minimum weight spanning tree by Kruskal's algorithm */
		/* (You may add extra functions if necessary) */
		
		/* ... Your code here ... */
		
		
		
		/* Add the weight of each edge in the minimum weight spanning tree
		   to totalWeight, which will store the total weight of the tree.
		*/
		/* ... Your code here ... */
		
		return totalWeight;
		
	}
	

	
		
	/* main()
	   Contains code to test the MWST function. You may modify the
	   testing code if needed, but nothing in this function will be considered
	   during marking, and the testing process used for marking will not
	   execute any of the code below.
	*/
	
public static class Edge implements Comparable<Edge>{

	private final int v;
	private final int w;
	private final double weight;
	
	public Edge(int v, int w, double weight){
		this.v=v;
		this.w=w;
		this.weight=weight;
	}
	
	public double weight(){
		return this.weight;
	
	}
	
	public int either(){
		return this.v;
	}
	
	public int other(int other){
		if(other ==v) return this.w;
		if(other ==w) return this.v;
		else throw new IllegalArgumentException("Not a valid endpoint");
	}
	
	public int compareTo(Edge that){
		if(this.weight()<that.weight()) return -1;
		if(this.weight()>that.weight()) return 1;
		else return 0;
		
	
	}


}
	
public static class UF{
	public int [] id;
	public byte [] rank;
	public int components;
	
	public UF(int N) {
        if (N < 0) throw new IllegalArgumentException();
        components = N;
        id = new int[N];
        rank = new byte[N];
        for (int i = 0; i < N; i++) {
            id[i] = i;
            rank[i] = 0;
        }
    }
	
	public int find(int p) {
        if (p < 0 || p >= id.length) throw new IndexOutOfBoundsException();
        while (p != id[p]) {
            id[p] = id[id[p]];    // path compression by halving
            p = id[p];
        }
        return p;
    }
	
	public boolean connected(int p, int q) {
        return find(p) == find(q);
    }
	
	public void union(int p, int q) {
        int i = find(p);
        int j = find(q);
        if (i == j) return;
        if      (rank[i] < rank[j]) id[i] = j;
        else if (rank[i] > rank[j]) id[j] = i;
        else {
            id[j] = i;
            rank[i]++;
        }
        components--;
    }



}

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
			
			int totalWeight = MWST(G);
			long endTime = System.currentTimeMillis();
			totalTimeSeconds += (endTime-startTime)/1000.0;
			
			System.out.printf("Graph %d: Total weight is %d\n",graphNum,totalWeight);
		}
		graphNum--;
		System.out.printf("Processed %d graph%s.\nAverage Time (seconds): %.2f\n",graphNum,(graphNum != 1)?"s":"",(graphNum>0)?totalTimeSeconds/graphNum:0);
	}
}