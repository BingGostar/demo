#include "timestamp.hpp"
#include <string>

using namespace std;
using namespace StBase;


int main() {
    Timestamp a = Timestamp::Now();
    a = a + Duration::Minute * 4;
    
}