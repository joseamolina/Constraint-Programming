
#include <gecode/driver .hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
using namespace Gecode;

class MapColoring : public IntMinimizeScript { 
	protected :
		IntVarArray regions ; // array of digits 
		IntVar num_colors ; // difference to minimize 
		static const int n = 18; // number of countries

public :
	enum types_branch { 
		DEGREE,
		SIZE , 
		DEGREE_SIZE, 
		AFC_SIZE, 
		ACTION_SIZE, 
		ACTION_SIZE_MAX, 
		AFC_MAX
	};
	int connections [18][18] = {
			{0 ,0 ,1 ,1 ,1 ,0 ,0 ,1 ,1 ,1 ,1 ,1 ,0 ,0 ,1 ,1 ,1 ,1} ,
			{0 ,0 ,1 ,0 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,0 ,1 ,1 ,1 ,0 ,1 ,0} ,
			{0 ,0 ,0 ,1 ,1 ,1 ,1 ,0 ,0 ,1 ,1 ,1 ,1 ,0 ,0 ,1 ,1 ,1} ,
			{0 ,0 ,0 ,0 ,1 ,1 ,0 ,1 ,1 ,0 ,1 ,0 ,1 ,1 ,1 ,0 ,1 ,1} ,
			{0 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,1 ,1 ,0 ,0 ,1 ,0 ,1 ,1 ,0 ,0} ,
			{0 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,1 ,1 ,0 ,0 ,1 ,0 ,1 ,1 ,0 ,0} ,
			{0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,1 ,1 ,1 ,1 ,0 ,0 ,1 ,1 ,1 ,1} ,
			{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,1 ,1 ,1 ,1 ,1 ,0 ,1 ,1} ,
			{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,1 ,1 ,1 ,0 ,0 ,1 ,1 ,1} ,
			{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,1 ,1 ,1 ,1 ,0 ,1 ,1} ,
			{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,1 ,0 ,1 ,1 ,0 ,0} ,
			{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,1 ,1 ,1 ,0 ,1} ,
			{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,1 ,1 ,1} ,
			{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,0} ,
			{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,1 ,1} ,
			{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,1} ,
			{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0} ,
			{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
            
	};

	MapColoring(const SizeOptions& opt): IntMinimizeScript(opt),
	regions(∗this , n ,0, n−1), num_colors(∗this , 2, n) { 
		int used[] = {};

		for(int i = 0; i < n; i++) { 
			int initer = 0;
			for(int j = 0; j < n; j++) {
				if (connections[i][j] != 0) {
					// Each edge
					rel(∗this, regions[i] != regions[j], opt.ipl());
				} else { 
					initer++;
				} 
			}
			if (initer == 0){
				// Symmetry breaking 1
				rel(∗this, regions[i] == 0, opt.ipl()); 
			}
		}
		// Bounds consistency: to be the largest
		max(∗this, regions, num_colors, opt.ipl()); 

		// Symmetry breaking 2
		rel(∗this, regions[0] == 0, opt.ipl()); 

		switch (opt.branching()) {
			case DEGREE_SIZE:
				branch(∗this, regions, INT_VAR_DEGREE_SIZE_MAX() ,INT_VAL_MIN()); 
				break;
			case ACTION_SIZE_MAX: 
				branch(∗this, regions, INT_VAR_ACTION_MAX(opt.decay()) , INT_VAL_MIN());
				break;
			case ACTION_SIZE: 
				branch(∗this, regions, INT_VAR_ACTION_SIZE_MAX(opt.decay()), INT_VAL_MIN ( ) ) ;
				break; 
			case SIZE:
				branch(∗this, regions, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
				break ;
			case AFC_SIZE: 
				branch(∗this, regions, INT_VAR_AFC_SIZE_MAX(opt.decay()), INT_VAL_MIN());
				break; 
			case AFC_MAX:
				branch(∗this, regions, INT_VAR_AFC_SIZE_MAX(opt.decay()), INT_VAL_MIN());
				break;
			case DEGREE:
				branch(∗this, regions, tiebreak(INT_VAR_DEGREE_MAX(), INT_VAR_SIZE_MIN()), INT_VAL_MIN());
				break ;

	virtual IntVar cost (void) const {
		return num_colors;
	}

	void print(std::ostream& os) const {
	os << regions << " num_colors: " << num_colors << std::endl;
	}

	MapColoring(MapColoring& s): IntMinimizeScript(s) { 
		num_colors.update(∗this, s.num_colors); 
		regions.update(∗this, s.regions);
	}

	// Copy during cloning
	virtual Space∗ copy () {
		return new MapColoring(∗this);
	} 

};


int main() {
	IntPropLevel props [3] = {IPL_VAL, IPL_DOM, IPL_BND};
	MapColoring::types_branch branchers[7] = {MapColoring::SIZE, MapColoring::ACTION_SIZE, MapColoring::DEGREE_SIZE, MapColoring::AFC_SIZE,
												MapColoring::DEGREE, MapColoring::ACTION_SIZE_MAX, MapColoring::AFC_MAX};
	
	char∗ props_t[3] = {"Value propagation" , "Domain Propagation" , "Bounds propagation"};
	char∗ branchers_t [7] = {"INT_VAR_SIZE_MIN and INT_VAL_MIN", "INT_VAR_ACTION_SIZE_MAX with decay and INT_VAL_MIN", "INT_VAR_DEGREE_SIZE_MAX and INT_VAL_MIN", "INT_VAR_AFC_SIZE_MAX with decay and INT_VAL_MIN()",
								"a tie break with INT_VAR_DEGREE_MAX and INT_VAR_SIZE_MIN along with INT_VAL_MIN",
								"INT_VAR_ACTION_MAX with decay and INT_VAL_MIN", "INT_VAR_AFC_SIZE_MAX with decay and INT_VAL_MIN"};

	for (int prop = 0; prop < 3; prop++){ 
		for (int br = 0; br < 7; br++){
			SizeOptions opt("Map coloring");
			std::cout << branchers_t[br] << std::endl; 
			std::cout << props_t[prop] << std::endl; 
			opt.solutions(0);
			opt.branching(branchers[br]); 
			opt.ipl(props[prop]);
			IntMinimizeScript::run<MapColoring ,BAB, SizeOptions >(opt);
		}
	}
	return 0;
}
