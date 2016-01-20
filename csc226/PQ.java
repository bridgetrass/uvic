import java.util.*;

public class PQ<T>{
private int size;
final private List<Node> heap;
final private Comparator<T>value;

private class Node{
	public T element;
	public int position;

}

public Heap( Comparator<T> value ) {
	size=0;
	Heap = new ArrayList<Node>();
	
	this.value = value;
}

public void insert(final T element){
	size++;
	    Node node = new Node();
    node.element = element;
    node.position = size-1;
    heap.add(node);
    decreaseKey( node );


}
  public final void clear() {
    heap.clear();
    size = 0;
  }
  
    public final T first() {
		return heap.get(0).element;
  }
  
    public void T pop() {
		T returnNode = first();
		swap( 0, size-1 );
		heap.remove(size-1);
		size--;
	
		
		if (size>0) {
			minHeapify( heap.get(0) );
		}
		return returnNode;
	}
	
	
	public final int size() {
		return size;
	}
	
	private final boolean decreaseKey( final Node node ) {
		int index = node.position;
		boolean modified = false;
	
	
	while ( index>0 &&  comparator.compare(heap.get(parent(index)).element, heap.get(index).element ) >= 0 ) {
		swap( index, parent(index) );
		index = parent(index);
		modified = true;
	}
		return modified;
    }
	
	private final void minHeapify(final Node node){
		int small;
		int index=node.position;
		int left=left(index);
		int right = right(index);
			if (left<size && comparator.compare(heap.get(left).element, heap.get(index).element) <= 0 ){
				small=left;
			}
			else{
				small=index;
			}
			
			if (right<size && comparator.compare(heap.get(right).element, heap.get(small).element ) <=0 ){
				small=right;
			}
			
			if (small!=index){
				swap(index,small);
				minHeapify(heap.get(small));
			}
		
	}
	
	private final void swap(int i, int j){
		Node temp=heap.get(i);
		temp.position=j;
		
		Node temp2=heap.get(j);
		temp2.position=i;
		
		heap.set(i,temp2);
		heap.set(j, temp);
	}
	
	
		
			
			
			
			
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  