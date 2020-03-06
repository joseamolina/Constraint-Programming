#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>
#include "searchEngine.cpp"
using namespace Gecode;
using namespace std;

enum ATT {
    COLOR = 0,
    PET = 1,
    DRINK = 2,
    TOBBACCO = 3,
    NATIONALITY = 4
};

enum COLOR {
    RED = 0,
    GREEN = 1,
    BLUE = 2,
    IVORY = 3,
    YELLOW = 4
};

enum NATION {
    ENGLISH = 0,
    SPANISH = 1,
    NORWEGIAN = 2,
    UKRANIAN = 3,
    JAPANESE = 4
};

enum DRINK {
    TEA = 0,
    ORANGE = 1,
    MILK = 2,
    COFFEE = 3
};

enum TOBACCO {
    PARLIAMENT = 0,
    OLD_GOLD = 1,
    KOOLS = 2,
    LUCKY_STRIKE = 3,
    CHESTERFIELD = 4
};

enum PET {
    DOG = 0,
    CAT = 1,
    SNAIL = 2,
    HORSE = 3,
    FOX = 4
};

const int TOTAL_SIZE = 9;

static string names[5] = {"COLOR", "PET", "DRINK", "TOBBACCO", "NATION"};
static string nationalities[5] = {"English", "Spaniard", "Norwegian", "Ukranian", "Japanese"};


class EinsteinProblem : public Space {

protected :
    // Number of people and attributes
    const int n = 5;

    // Non-static set of values
    IntVarArray X;

public :

    EinsteinProblem(void) : X(*this, n * n , 0, n - 1) {

        // Initial structured matrix. A copy pointer of X array.
        Matrix<IntVarArray> matrix_composed(X, n, n);

        // The Englishman lives in the red house.
        rel(*this, matrix_composed(NATIONALITY, ENGLISH) == matrix_composed(COLOR, RED));

        // Coffee is drunk in the green house.
        rel(*this, matrix_composed(DRINK, COFFEE) == matrix_composed(COLOR, GREEN));

        // The Norwegian lives next to the blue house.
        rel(*this, abs(matrix_composed(NATIONALITY, NORWEGIAN) - matrix_composed(COLOR, BLUE)) == 1);

        // Kools are smoked in the yellow house.
        rel(*this, matrix_composed(TOBBACCO, KOOLS) == matrix_composed(COLOR, YELLOW));

        // The Spaniard owns the dog.
        rel(*this, matrix_composed(NATIONALITY, SPANISH) == matrix_composed(PET, DOG));

        // The Old Gold smoker owns snails.
        rel(*this, matrix_composed(TOBBACCO, OLD_GOLD) == matrix_composed(PET, SNAIL));

        // The Ukrainian drinks tea.
        rel(*this, matrix_composed(NATIONALITY, UKRANIAN) == matrix_composed(DRINK, TEA));

        // The Lucky Strike smoker drinks orange juice.
        rel(*this, matrix_composed(TOBBACCO, LUCKY_STRIKE) == matrix_composed(DRINK, ORANGE));

        // The green house is immediately to the right of the ivory house.
        rel(*this, matrix_composed(COLOR, GREEN) == matrix_composed(COLOR, IVORY) + 1);

        // Milk is drunk in the middle house.
        rel(*this, matrix_composed(DRINK, MILK) == 2);

        // The Norwegian lives in the first house on the left.
        rel(*this, matrix_composed(NATIONALITY, NORWEGIAN) == 0);

        // The man who smokes Chesterfields lives in the house next to the man with the fox.
        rel(*this, abs(matrix_composed(TOBBACCO, CHESTERFIELD) - matrix_composed(PET, FOX)) == 1);

        // Kools are smoked in the house next to the house where the horse is kept.
        rel(*this, abs(matrix_composed(TOBBACCO, KOOLS) - matrix_composed(PET, HORSE)) == 1);

        // The Japanese smoke Parliaments.
        rel(*this, matrix_composed(NATIONALITY, JAPANESE) == matrix_composed(TOBBACCO, PARLIAMENT));

        // Column distiction. Each Column attribute must be different in all cases.
        for (int i = 0; i < n; i++) {
            distinct(*this, matrix_composed.col(i));

        }

        // Heuristic for branching.
        branch(*this, X, INT_VAR_SIZE_MIN(), INT_VAL_SPLIT_MIN());

    }

    EinsteinProblem(EinsteinProblem& s) : Space(s) {
        X.update(*this , s.X);
    }

    virtual Space* copy() {
        return new EinsteinProblem(*this);
    }

    void print(ostream& os) const {
        // Matrix-wrapper for the square
        Matrix<IntVarArray> m(X, n, n);
        cout << "\nFormulation based on positions.\nEach number belongs to the position\nof the house" << endl;
        for (int i = 0; i<n; i++) {
            string espace = "";
            for (int sp = 0; sp<TOTAL_SIZE - names[i].size(); sp++) espace += " ";
            os << "\t" << names[i] << ":" << espace;
            for (int j = 0; j<n; j++)
                os << m(i,j) << " ";
            os << std::endl;
        }
        os << std::endl;

        int cat = m(PET, CAT).val();

        for (int na = 0; na < n; na++){
            if (m(NATIONALITY, na).val() == cat){
                cout << "The cat is kept by the " << nationalities[na] << endl;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    EinsteinProblem* m = new EinsteinProblem;
    SimpleSearchTracer* t = new SimpleSearchTracer;

    Search::Options o;
    o.tracer = t;

    DFS<EinsteinProblem> e(m, o);
    delete m;

    if (EinsteinProblem* s = e.next()) {
        s->print(cout);
        delete s;
    }
}