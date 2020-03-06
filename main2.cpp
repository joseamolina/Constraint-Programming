#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>
#include "searchEngine.cpp"

using namespace std;
using namespace Gecode;

namespace {

    const int budget_max = 225;
    const int staff_max = 28;
    const short projects_max = 9;
    const int n_projects = 15;

    const int not_with[15] = {9, -1, -1, -1, 5, 4, -1, -1, -1, 0, 14, -1, -1, -1, 10};

    const int required[15] = {-1, -1, 14, 14, -1, -1, -1, 6, -1, -1, -1, -1, 1, 1, -1};

    const string project_names[15] = {"Ishall", "SportHall", "Hotell", "Restaurang", "Kontor A", "KontorB", "Skola", "Dagis",
                                        "Lager", "Simhall", "Hyreshus", "Bilverkstad", "TennisHall", "Idrottsanl", "BAYthamn"};
    IntArgs value( {600, 400, 100, 150, 80, 120, 200, 220, 90, 380, 290, 130, 80, 270, 280 } );
    IntArgs budget({ 35, 34, 26, 12, 10, 18, 32, 11, 10, 22, 27, 18, 16, 29, 22 });
    IntArgs staff({5, 3, 4, 2, 2, 2, 4, 1, 1, 5, 3, 2, 2, 4, 3});


}

class MaximizationProblem : public Space {
protected:
    BoolVarArray projects;
public:

    MaximizationProblem(void) : projects(*this,  n_projects, 0, 1) {

        linear(*this, budget, projects, IRT_LE, budget_max);
        linear(*this, staff, projects, IRT_LE, staff_max);
        linear(*this, projects, IRT_LQ, projects_max);

        for (int i = 0; i < 15; i++) {
            if (not_with[i] != -1) rel(*this, projects[i], BOT_AND, projects[not_with[i]], 0);

            if (required[i] != -1) rel(*this, projects[i], BOT_OR, projects[required[i]], 1);
        }

        branch(*this, projects, BOOL_VAR_DEGREE_MIN(), BOOL_VAL_MAX());
    }

    virtual void constrain(const Space& _b) {
        const MaximizationProblem &b = static_cast<const MaximizationProblem &>(_b);

        int valueB = 0;
        BoolVarArray projectsB = b.projects;

        for (int i = 0; i < n_projects; i++){
            valueB += projectsB[i].val() * value[i];
        }

        linear(*this, value, projects, IRT_GQ, valueB);
    }

    // Print solution
    virtual void print(std::ostream& os) const {

        int investment = 0;

        string project_caught = "\nWe will invest in these projects: ";
        for (int j = 0; j < n_projects; j++){
            investment += projects[j].val() * value[j];
            project_caught += projects[j].val() ? project_names[j] + " " + " (€" + to_string(value[j]) + "M), " : "";
        }

        cout << projects;

        cout << project_caught << endl;
        cout << "With a return value of €" << investment << "M" << endl;
    }

    MaximizationProblem(MaximizationProblem& s) : Space(s) {
        projects.update(*this , s.projects);
    }

    virtual Space* copy() {
        return new MaximizationProblem(*this);
    }
};

int main(int argc, char* argv[]) {

    MaximizationProblem* m = new MaximizationProblem();
    // SimpleSearchTracer* t = new SimpleSearchTracer();

    Search::Options o;
    // o.tracer = t;

    BAB<MaximizationProblem> e(m);
    delete m;

    MaximizationProblem* new_copy;
    while (MaximizationProblem* s = e.next()) {

        new_copy = s;
        new_copy->print(cout);
    }

    // new_copy->print(cout);
}