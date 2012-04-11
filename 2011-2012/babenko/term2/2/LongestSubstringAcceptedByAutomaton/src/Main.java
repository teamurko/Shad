import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.StringTokenizer;

public class Main implements Runnable {

	public static void main(String[] args) {
		new Thread(new Main()).start();
	}

	public void run() {
		try {
			solve();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	class Edge {
		Edge(int from, int to, char label) {
			this.from = from;
			this.to = to;
			this.label = label;
		}

		public int from, to;
		public char label;
	}

	class Arc {
		Arc(int to, char label) {
			this.to = to;
			this.label = label;
		}

		public int to;
		public char label;
	}

	class Automaton {
		Automaton(int numVertices, List<Edge> edges, List<Integer> terminals) {
			this.numVertices = numVertices;
			this.edges = edges;
			this.isTerminal = new boolean[numVertices];
			graph = new ArrayList[numVertices];
			for (int vertex = 0; vertex < numVertices; ++vertex) {
				graph[vertex] = new ArrayList<Arc>();
			}
			for (Edge edge : edges) {
				graph[edge.from].add(new Arc(edge.to, edge.label));
			}
			for (int vertex : terminals) {
				isTerminal[vertex] = true;
			}
		}

		public List<Integer> next(int vertex, char label) {
			if (transitions == null) {
				buildTransitions();
			}
			return transitions[vertex][getLabelId(label)];
		}

		public boolean isTerminal(int vertex) {
			return isTerminal[vertex];
		}

		public int numVertices() {
			return numVertices;
		}

		private int getLabelId(char character) {
			if (character == EPSILON) {
				return ALPH_NUM - 1;
			} else if (Character.isLetter(character)) {
				if (Character.toLowerCase(character) != character) {
					throw new RuntimeException(character
							+ " is not a lowercase letter.");
				} else {
					return character - 'a';
				}
			} else {
				throw new RuntimeException("Unknown character : " + character);
			}
		}

		private void buildTransitions() {
			// FIXME
			transitions = new ArrayList[numVertices][ALPH_NUM];
			for (int vertex = 0; vertex < numVertices; ++vertex) {
				for (int letter = 0; letter < ALPH_NUM; ++letter) {
					transitions[vertex][letter] = new ArrayList<Integer>();
				}
				for (Arc arc : graph[vertex]) {
					transitions[vertex][getLabelId(arc.label)].add(arc.to);
				}
			}
		}
		
		public int initVertex() {
			return initVertex;
		}

		public void shrinkByEpsilonArcs() {
			used = new boolean[numVertices];
			for (int vertex = 0; vertex < numVertices; ++vertex) {
				used[vertex] = false;
			}
			verticesInOrder = new ArrayList<Integer>();
			for (int vertex = 0; vertex < numVertices; ++vertex) {
				if (!used[vertex]) {
					dfs(vertex, graph);
				}
			}
			for (int vertex = 0; vertex < numVertices; ++vertex) {
				used[vertex] = false;
			}
			List<Arc>[] transposedGraph = new ArrayList[numVertices];
			for (int vertex = 0; vertex < numVertices; ++vertex) {
				transposedGraph[vertex] = new ArrayList<Arc>();
			}
			for (Edge edge : edges) {
				transposedGraph[edge.to].add(new Arc(edge.from, edge.label));
			}
			int numComponents = 0;
			componentIndex = new int[numVertices];
			Collections.reverse(verticesInOrder);
			for (int vertex : verticesInOrder) {
				if (!used[vertex]) {
					dfs(vertex, transposedGraph, numComponents++);
				}
			}
			rebuildGraph(numComponents);
		}

		private void rebuildGraph(int numComponents) {
			graph = new ArrayList[numComponents];
			for (int vertex = 0; vertex < numComponents; ++vertex) {
				graph[vertex] = new ArrayList<Arc>();
			}
			for (Edge edge : edges) {
				edge.from = componentIndex[edge.from];
				edge.to = componentIndex[edge.to];
				if (edge.label != EPSILON || edge.from != edge.to) {
					graph[edge.from].add(new Arc(edge.to, edge.label));
				}
			}
			boolean[] isTerminal = new boolean[numComponents];
			for (int vertex = 0; vertex < numVertices; ++vertex) {
				if (this.isTerminal[vertex]) {
					isTerminal[componentIndex[vertex]] = true;
				}
			}
			this.isTerminal = isTerminal;

			numVertices = numComponents;
			initVertex = componentIndex[0];
		}

		private void dfs(int vertex, List<Arc>[] graph, int component) {
			used[vertex] = true;
			for (Arc arc : graph[vertex]) {
				if (arc.label == EPSILON && !used[arc.to]) {
					dfs(arc.to, graph, component);
				}
			}
			componentIndex[vertex] = component;
		}

		private void dfs(int vertex, List<Arc>[] graph) {
			used[vertex] = true;
			for (Arc arc : graph[vertex]) {
				if (arc.label == EPSILON && !used[arc.to]) {
					dfs(arc.to, graph);
				}
			}
			verticesInOrder.add(vertex);
		}

		public static final int ALPH_NUM = 27;
		private boolean[] isTerminal;
		private boolean[] used;
		private int[] componentIndex;
		private int numVertices;
		private int initVertex;
		private List<Integer> verticesInOrder;
		private List<Edge> edges;
		private List<Arc>[] graph;
		private List<Integer>[][] transitions;
	}

	private Automaton readAutomaton(TokenReader reader) throws IOException {
		int numVertices = reader.nextInt();
		int numEdges = reader.nextInt();
		int numTerminals = reader.nextInt();

		List<Integer> terminals = new ArrayList<Integer>();
		for (int index = 0; index < numTerminals; ++index) {
			terminals.add(reader.nextInt());
		}

		List<Edge> edges = new ArrayList<Edge>();
		for (int edgeId = 0; edgeId < numEdges; ++edgeId) {
			int from = reader.nextInt();
			char label = reader.nextString().charAt(0);
			int to = reader.nextInt();
			edges.add(new Edge(from, to, label));
		}

		return new Automaton(numVertices, edges, terminals);
	}

	class TokenReader {
		TokenReader(BufferedReader reader) {
			this.reader = reader;
		}

		public int nextInt() throws IOException {
			update();
			return Integer.parseInt(tokenizer.nextToken());
		}

		public String nextString() throws IOException {
			update();
			return tokenizer.nextToken();
		}

		private void update() throws IOException {
			while (tokenizer == null || !tokenizer.hasMoreTokens()) {
				tokenizer = new StringTokenizer(reader.readLine());
			}
		}

		private BufferedReader reader;
		private StringTokenizer tokenizer;
	}

	class LongestSubstringFinder {
		LongestSubstringFinder(Automaton automaton, String string) {
			this.automaton = automaton;
			this.string = string;
			this.longestPathLength = new int[automaton.numVertices()][string
					.length() + 1];
			this.nextVertex = new int[automaton.numVertices()][string.length() + 1];
			this.nextPosition = new int[automaton.numVertices()][string
					.length() + 1];
			for (int[] array : longestPathLength) {
				Arrays.fill(array, UNDEFINED);
			}
			for (int[] array : nextVertex) {
				Arrays.fill(array, NON_EXISTENT);
			}
		}

		private int longestPathLength(int vertex, int position) {
			if (longestPathLength[vertex][position] != UNDEFINED) {
				return longestPathLength[vertex][position];
			}

			int result = NON_EXISTENT;
			int nextVertex = NON_EXISTENT;
			int nextPosition = NON_EXISTENT;
			if (automaton.isTerminal(vertex)) {
				result = 0;
			}
			for (int adjacentVertex : automaton.next(vertex, EPSILON)) {
				int candidateValue = longestPathLength(adjacentVertex, position);
				if (candidateValue != NON_EXISTENT) {
					if (result < candidateValue) {
						result = candidateValue;
						nextVertex = adjacentVertex;
						nextPosition = position;
					}
				}
			}
			if (position < string.length()) {
				for (int adjacentVertex : automaton.next(vertex, string.charAt(position))) {
					int candidateValue = longestPathLength(adjacentVertex, position + 1);
					if (candidateValue != NON_EXISTENT) {
						if (result < candidateValue + 1) {
							result = candidateValue + 1;
							nextVertex = adjacentVertex;
							nextPosition = position + 1;
						}
					}
				}
			}

			this.nextVertex[vertex][position] = nextVertex;
			this.nextPosition[vertex][position] = nextPosition;
			return longestPathLength[vertex][position] = result;
		}

		// FIXME final
		private String find() {
			if (answer == null) {
				answer = restoreSubstring();
			}
			return answer;
		}

		private String restoreSubstring() {
			int maxLength = 0;
			int argMaxPosition = 0;
				for (int position = 0; position < string.length(); ++position) {
					if (maxLength < longestPathLength(automaton.initVertex(), position)) {
						maxLength = longestPathLength(automaton.initVertex(), position);
						argMaxPosition = position;
					}
				}
			StringBuffer result = new StringBuffer();
			if (maxLength > 0) {
				int vertex = automaton.initVertex();
				int position = argMaxPosition;
				while (nextVertex[vertex][position] != NON_EXISTENT) {
					if (position + 1 == nextPosition[vertex][position]) {
						result.append(string.charAt(position));
					}
					int newVertex = nextVertex[vertex][position];
					position = nextPosition[vertex][position];
					vertex = newVertex;
				}
			}
			assert (result.length() == maxLength);
			return result.toString();
		}

		private static final int UNDEFINED = -2;
		private static final int NON_EXISTENT = -1;

		private int[][] longestPathLength;
		private int[][] nextVertex;
		private int[][] nextPosition;
		private Automaton automaton;
		private String string;
		private String answer;
	}

	private final char EPSILON = '$';

	private void solve() throws IOException {
		//BufferedReader reader = new BufferedReader(new FileReader(new File(
		//		"input.txt")));
		BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
		TokenReader tokenReader = new TokenReader(reader);

		Automaton automaton = readAutomaton(tokenReader);
		automaton.shrinkByEpsilonArcs();

		LongestSubstringFinder solver = new LongestSubstringFinder(automaton,
				tokenReader.nextString());
		String answer = solver.find();

		if ("".equals(answer)) {
			System.out.println("No solution");
		} else {
			System.out.println(answer);
		}
		System.out.close();
	}

}
