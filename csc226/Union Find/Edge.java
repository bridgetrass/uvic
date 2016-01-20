import java.util.Comparator;
import java.util.*;

public class Edge implements Comparable<Edge>{

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