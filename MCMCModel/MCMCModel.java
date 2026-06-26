package org.j3lsmp.categorization_modeling.complete.utilitiesNew;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Random;
import java.util.Map.Entry;
import java.util.regex.Pattern;
import java.util.stream.Collectors;

public class MCMCModel implements Model {
	
	
	static final int DEPTH = 5000;
	static final int VARS = 5;
	static final int OUTPUT_GROUPS = 2; //is there a way to do this with more output groups? Depends how we calculate simplicity, but I'm not sure. For now, unimplemented.
	
	static final int[] maxPIsForVars = {0, 1, 2, 5, 9, 32, 92, 432, 1584, 5832};
	
	List<SeenExample> seenExamples = new ArrayList<>();
	
	int[] currentTruthTable = new int[(int) Math.pow(2, VARS)];
	
	Random rand = new Random();
	
	MCMCModel() {
		recalculateTruthTable();
	}
	
	private void recalculateTruthTable() {
		currentTruthTable = new int[(int) Math.pow(2, VARS)]; //default to all false table
		double currentScore = score(currentTruthTable, seenExamples);
		
		int[] bestTable = Arrays.copyOf(currentTruthTable, currentTruthTable.length);
		double bestScore = currentScore;
		
		for(int i = 0; i < DEPTH; i++) {
			int[] proposal = modifyTable(currentTruthTable);
			double newScore = score(proposal, seenExamples);
			double diff = newScore - bestScore;
			if (diff < rand.nextDouble()) { // if new table is better (lower score) diff will be negative and this will always be true. Otherwise, more likely the less worse proposal is
				currentTruthTable = proposal;
				currentScore = newScore;
				if (currentScore < bestScore) {
					bestTable = currentTruthTable;
					bestScore = currentScore;
				}
			}
		}
		
		currentTruthTable = bestTable;
	}

	@Override
	public void learnCorrectCategorization(boolean[] traits, boolean isEvil) {
		seenExamples.add(new SeenExample(traits, isEvil));
		if (seenExamples.size() > 10)
			seenExamples.remove(0);
		recalculateTruthTable();
	}

	@Override
	public double getCertainty(boolean[] traits) {
		int row = 0;
		for (boolean trait : traits) {
			row |= trait ? 1 : 0;
			row <<= 1;
		}
		row >>= 1;
		return currentTruthTable[row] == 1 ? 100 : 0;
	}
	
	int[] modifyTable(int[] truthTable) {
		int[] proposal = Arrays.copyOf(truthTable, truthTable.length);
		
		int rowsToChange = rand.nextInt(currentTruthTable.length);
		
		Integer[] distinctIndices = new Integer[currentTruthTable.length];
		for (int i = 0; i < distinctIndices.length; i++)
			distinctIndices[i] = i;
		
		List<Integer> indicesList = Arrays.stream(distinctIndices).collect(Collectors.toList());
		Collections.shuffle(indicesList);
		List<Integer> selection = indicesList.subList(0, rowsToChange);
		
		for (Integer index : selection)
			proposal[index] = proposal[index] == 1 ? 0 : 1;
		
		return proposal;
	}
	
	//LOW MEANS BETTER. Normalize to [0,1]
	double score(int[] truthTable, List<SeenExample> examples) {
		return simplicityScore(truthTable) + accuracyScore(truthTable, examples);
	}
	
	double simplicityScore(int[] truthTable) {
		//do a little QMC:
		ArrayList<String> minterms = new ArrayList<>();
		for (int row = 0; row < truthTable.length; row++)
			if (truthTable[row] == 1) {
				String binary = Integer.toBinaryString(row);
				while (binary.length() < VARS)
					binary = "0" + binary;
				minterms.add(binary);
			}
		ArrayList<String> primeImplicants = getPrimeImplicants(minterms);
		HashMap<String, String> primeImplicantChart = new HashMap<>();
		for (int i = 0; i < primeImplicants.size(); i++)
			primeImplicantChart.put(primeImplicants.get(i), "");
		for (String primeImplicant : primeImplicantChart.keySet()) {
			Pattern regex = convertToRegularExpression(primeImplicant);
			for (int j = 0; j < minterms.size(); j++) {
				String minterm = minterms.get(j);
				if (regex.matcher(minterm).matches())
					primeImplicantChart.put(primeImplicant, primeImplicantChart.get(primeImplicant) + "1");
				else
					primeImplicantChart.put(primeImplicant, primeImplicantChart.get(primeImplicant) + "0");
			}
		}
		ArrayList<String> essentialPrimeImplicants = new ArrayList<>();
		ArrayList<Entry<String, String>> mintermCoverages = new ArrayList<>(primeImplicantChart.entrySet());
		for (int i = 0; i < getLengthOrDefault(mintermCoverages); i++) {
			int count = 0;
			String recentKey = "";
			for (int j = 0; j < mintermCoverages.size(); j++) {
				String coverage = mintermCoverages.get(j).getValue();
				if (coverage.charAt(i) == '1') {
					count++;
					recentKey = mintermCoverages.get(j).getKey();
				}
			}
			if (count == 1 && !essentialPrimeImplicants.contains(recentKey))
				essentialPrimeImplicants.add(recentKey);
		}
		for (String essentialPrimeImplicant : essentialPrimeImplicants) {
			String coverage = primeImplicantChart.get(essentialPrimeImplicant);
			for (int i = 0; i < coverage.length(); i++)
				if (coverage.charAt(i) == '1')
					minterms.set(i, null);
		}
		for (int i = 0; i < minterms.size(); i++) {
			if (minterms.get(i) == null)
				continue;
			int count = -1;
			String recentKey = "";
			for (int j = 0; j < primeImplicantChart.size(); j++) {
				String primeImplicant = primeImplicantChart.keySet().toArray()[j].toString();
				String coverage = primeImplicantChart.get(primeImplicant);
				if (coverage.charAt(i) == '1') {
					int countHyphens = (int) primeImplicant.chars().filter(ch -> ch == '-').count();
					if (countHyphens > count)
						recentKey = primeImplicant;
				}
			}
			if (!essentialPrimeImplicants.contains(recentKey))
				essentialPrimeImplicants.add(recentKey);
		}
		
		int maxSpecificityInPI = 0;
		for (String essentialPrimeImplicant : essentialPrimeImplicants) {
			int count = 0;
			for (int i = 0; i < essentialPrimeImplicant.length(); i++)
				if (essentialPrimeImplicant.charAt(i) != '-')
					count++;
			if (count > maxSpecificityInPI)
				maxSpecificityInPI = count;
		}
		
		return 0.3 * essentialPrimeImplicants.size() / maxPIsForVars[VARS] + 0.2 * maxSpecificityInPI / VARS;
	}
	
	private static int getLengthOrDefault(ArrayList<Entry<String, String>> mintermCoverages) {
		try {
			return mintermCoverages.get(0).getValue().length();
		} catch (IndexOutOfBoundsException e) {
			return 0;
		}
	}
	
	private static Pattern convertToRegularExpression(String primeImplicant) {
		StringBuilder regex = new StringBuilder();
		for (int i = 0; i < primeImplicant.length(); i++)
			if (primeImplicant.charAt(i) == '-')
				regex.append("[01]");
			else
				regex.append(primeImplicant.charAt(i));
		return Pattern.compile(regex.toString());
	}
	
	private static ArrayList<String> getPrimeImplicants(ArrayList<String> minterms) {
		ArrayList<String> primeImplicants = new ArrayList<>();
		ArrayList<Boolean> merges = new ArrayList<>();
		while (merges.size() < minterms.size())
			merges.add(false);
		int numberOfMerges = 0;
		for (int i = 0; i < minterms.size(); i++)
			for (int c = i + 1; c < minterms.size(); c++) {
				String minterm1 = minterms.get(i), minterm2 = minterms.get(c);
				if (checkDashesAlign(minterm1, minterm2) && checkMintermDifference(minterm1, minterm2)) {
					String mergedMinterm = mergeMinterms(minterm1, minterm2);
					if (!primeImplicants.contains(mergedMinterm))
						primeImplicants.add(mergedMinterm);
					merges.set(i, true);
					merges.set(c, true);
					numberOfMerges++;
				}
			}
		
		for (int i = 0; i < minterms.size(); i++)
			if (!merges.get(i) && !primeImplicants.contains(minterms.get(i)))
				primeImplicants.add(minterms.get(i));
		
		if (numberOfMerges > 0)
			return getPrimeImplicants(primeImplicants);
		else
			return primeImplicants;
	}
	
	private static boolean checkDashesAlign(String minterm1, String minterm2) {
		for (int i = 0; i < minterm1.length(); i++)
			if (minterm1.charAt(i) == '-' && minterm2.charAt(i) != '-')
				return false;
		return true;
	}
	
	private static boolean checkMintermDifference(String minterm1, String minterm2) {
		int intMinterm1 = Integer.parseInt(minterm1.replace("-", "0"), 2);
		int intMinterm2 = Integer.parseInt(minterm2.replace("-", "0"), 2);
		
		int res = intMinterm1 ^ intMinterm2;
		return res != 0 && (res & (res - 1)) == 0;
	}
	
	private static String mergeMinterms(String minterm1, String minterm2) {
		StringBuilder mergedMinterm = new StringBuilder();
		for (int i = 0; i < minterm1.length(); i++)
			if (minterm1.charAt(i) == minterm2.charAt(i))
				mergedMinterm.append(minterm1.charAt(i));
			else
				mergedMinterm.append('-');
		return mergedMinterm.toString();
	}
	
	double accuracyScore(int[] truthTable, List<SeenExample> examples) {
		double ret = 0;
		for (SeenExample example : examples) {
			int index = 0;
			for (boolean trait : example.traits) {
				index |= trait ? 1 : 0;
				index <<= 1;
			}
			index >>= 1;
			if ((truthTable[index] == 1) != example.isEvil)
				ret += .5 / examples.size();
		}
		return ret;
	}
}

class SeenExample {
	boolean[] traits;
	boolean isEvil;
	
	SeenExample(boolean[] traits, boolean isEvil) {
		this.traits = traits;
		this.isEvil = isEvil;
	}
}