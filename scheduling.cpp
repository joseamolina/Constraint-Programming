#include <gecode/driver.hh>
#include <gecode/int.hh> 
#include <iostream>
#include <gecode/minimodel.hh> 
#include <numeric>
#include <algorithm> 
using namespace Gecode;
using namespace std; 
namespace{
	// Constants
	// The duration of all tasks 
	IntArgs durations({	54, 34, 61, 2,
						9, 15, 89, 70, 
						38, 19, 28, 87,
						95, 34, 7, 29});
	// Each machine
	int machinesA[] {0, 1, 2, 3};
	int machinesB[] {2, 1, 3, 0};
	int machinesC[] {1, 3, 0, 2};
	int machinesD[] {1, 0, 3, 2};

	// Permutations of 0, 1, 2, 3 for couples.
	int permA[] = {0, 0, 0, 1, 1, 2}; 
	int permB[] = {1, 2, 3, 2, 3, 3}; 
	int n_perm = 6;

	// Calculate the maximum make Span
	int res_red_total = accumulate(durations.begin(), durations .end() , 0);
	int n_jobs = 4;
	int n_tasks = 16;
}

class Scheduling : public IntMinimizeScript {
	// The variables used
	protected :

		// This array indicates the initial time of each task.
		IntVarArray initial;

		// This array indicates the end time of each task.
		IntVarArray final;

		// The last time to terminate whatever variable.
		IntVar makeSpan; 

	public :
		// Actual model
		Scheduling(const SizeOptions& opt) : IntMinimizeScript(opt),
			initial(∗this, n_tasks, 0, res_red_total),
			final(∗this, n_tasks, 2, res_red_total), 
			makeSpan(∗this , 2, res_red_total) {

				// Ensures that the final time is equal
				// than the initial time plus the durations
				for (int i = 0; i < n_tasks; i++) {
					rel(∗this, final[i] == (initial[i] + durations[i])); 
				}

				// Ensures to select the cases where
				max(∗this , final , makeSpan, IPL_DOM); 

				// For each job
				for (int i = 0; i < n_jobs; i++){

					// For each couple of permutation of tasks
					for (int r = 0; r < n_perm; r++) {
						// Get the index of the task
						int a = permA[r] + (i ∗ 4); 
						int b = permB[r] + (i ∗ 4);
				
						// Reify a and b
						BoolVar b1(∗this , 0, 1); 
						BoolVar b2(∗this , 0, 1);

						IntVar tmp0 = expr(∗this, initial[a] + durations[a]); 
						IntVar tmp1 = expr(∗this, initial[b] + durations[b]);
						rel(∗this, tmp0, IRT_LQ, initial[b], b1); rel(∗this, tmp1, IRT_LQ, initial[a], b2);
						rel(∗this, b1 != b2); 
					}
				}

				// For each
				for (int j = 0; j < n_jobs; j++){
					auto itrA = find(machinesA, machinesA + n_jobs, j);

					int indA = 0;
					if (itrA != end(machinesA)){
						indA = distance(machinesA, itrA ); 
					}

					auto itrB = find (machinesB , machinesB
					int indB = 0;
					if (itrB != end(machinesB)){
						indB = distance(machinesB, itrB); 
					}
					indB += 4;

					auto itrC = find(machinesC, machinesC + n_jobs, j);
					int indC = 0;
					if (itrC != end(machinesC)){
						indC = distance(machinesC , itrC ); 
					}
					indC += 8;

					auto itrD = find (machinesD, machinesD
					int indD = 0;
					if (itrD != end(machinesD)){
						indD = distance (machinesD , itrD ); 
					}
					indD += 12;

					int list_p_A[] = {indA, indA, indA, indB, indB, indC};
					int list_p_B[] = {indB, indC, indD, indC, indD, indD};

					for (int ind_t = 0; ind_t < n_perm; ind_t++) {

						int a = list_p_A[ind_t]; 
						int b = list_p_B[ind_t];

						BoolVar b1(∗this, 0, 1);
						BoolVar b2(∗this, 0, 1);

						IntVar tmp0 = expr(∗this, initial[a] + durations[a]);
						IntVar tmp1 = expr(∗this, initial[b] + durations[b]);

						rel(∗this, tmp0, IRT_LQ, initial[b], b1);
						rel(∗this, tmp1, IRT_LQ, initial[a], b2);

						rel(∗this, b1 != b2); 
					}
				}

				branch(∗this, initial, INT_VAR_DEGREE_SIZE_MIN(), INT_VAL_SPLIT_MIN());
				branch(∗this, final, INT_VAR_DEGREE_SIZE_MIN(), INT_VAL_SPLIT_MIN());
			}


	// Constructor for cloning s
	Scheduling(Scheduling& s) : IntMinimizeScript(s) { 
		initial.update(∗this, s.initial);
		final.update(∗this, s.final);
		makeSpan.update(∗this, s.makeSpan);

	}
	// Copy during cloning
	virtual Space∗ copy () {
		return new Scheduling(∗this);
	}

	// Print solution
	virtual void print(std::ostream& os) const {
		os << "start times    : " << initial << std::endl; os << "final times    : " << final << std::endl; os << "last_end_time: " << makeSpan << std::endl; 
		os << std::endl;
	}

	// Return cost
	virtual IntVar cost (void) const {
		return makeSpan;
	} 
};

int main() {
	SizeOptions opt("Scheduling" );
	Scheduling∗ m = new Scheduling(opt); 
	opt.solutions(0);

	RestartMode branchers[4] = {RM_LUBY, RM_GEOMETRIC, RM_NONE, RM_LINEAR}; 
	char∗ branchers_t[4] = {"Luby", "Geometric", "None", "Linear"};
	for (int good = 0; good < 2; good++){ 
		for (int str = 0; str < 4; str++) {
			cout << "No−goods: " << good << ", restart: " << branchers_t[str] << "." << endl;

			// Restarts
			opt.restart(branchers[str]);
			opt.restart_scale(1);
			opt.restart_base(1.5);

			// No goods
			opt.nogoods(good);
			opt.nogoods_limit(100);

			IntMinimizeScript::run<Scheduling, BAB, SizeOptions>(opt);

		} 
	}
	return 0;
}