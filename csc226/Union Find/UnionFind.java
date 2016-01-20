public class UF{
	public int id;
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
        count--;
    }



}