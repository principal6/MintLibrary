#include <MintLibrary/Include/Test.h>

#include <MintLibrary/Include/AllHeaders.h>
#include <MintLibrary/Include/AllHpps.h>


namespace mint
{
	namespace TestLibrary
	{
        bool test()
        {
            MINT_ASSURE(test_algorithm());
            return true;
        }

        bool test_algorithm()
        {
            using namespace mint;

            Vector<uint32> t0;
            t0.push_back(7);
            t0.push_back(9);
            t0.push_back(8);
            t0.push_back(6);
            t0.push_back(4);
            quickSort(t0, ComparatorAscending<uint32>());
            quickSort(t0, ComparatorDescending<uint32>());

            Vector<uint32> t1;
            t1.push_back(3);
            t1.push_back(2);
            t1.push_back(1);
            t1.push_back(4);
            t1.push_back(5);
            quickSort(t1, ComparatorAscending<uint32>());
            quickSort(t1, ComparatorDescending<uint32>());

            Vector<uint32> t2;
            t2.push_back(3);
            t2.push_back(2);
            t2.push_back(1);
            t2.push_back(6);
            t2.push_back(7);
            t2.push_back(5);
            quickSort(t2, ComparatorAscending<uint32>());
            quickSort(t2, ComparatorDescending<uint32>());

            Vector<uint32> t3;
            t3.push_back(1);
            t3.push_back(2);
            t3.push_back(3);
            t3.push_back(4);
            t3.push_back(5);
            quickSort(t3, ComparatorAscending<uint32>());
            quickSort(t3, ComparatorDescending<uint32>());

            Vector<uint32> t4;
            t4.push_back(4);
            t4.push_back(3);
            t4.push_back(5);
            t4.push_back(7);
            t4.push_back(6);
            t4.push_back(5);
            quickSort(t4, ComparatorAscending<uint32>());
            quickSort(t4, ComparatorDescending<uint32>());

            Vector<uint32> t5;
            t5.push_back(0);
            t5.push_back(0);
            t5.push_back(0);
            quickSort(t5, ComparatorAscending<uint32>());
            quickSort(t5, ComparatorDescending<uint32>());

            Vector<uint32> t6;
            t6.push_back(0);
            t6.push_back(0);
            t6.push_back(1);
            t6.push_back(0);
            t6.push_back(0);
            quickSort(t6, ComparatorAscending<uint32>());
            quickSort(t6, ComparatorDescending<uint32>());

            return true;
        }
	}
}
