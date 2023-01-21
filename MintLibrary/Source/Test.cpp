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

			struct StringWithComparator
			{
				StringWithComparator(const StringReferenceA& string) : _string{ string.c_str() } { __noop; }

				StackStringA<kMaxPath> _string;
				bool operator==(const StringWithComparator& rhs) const
				{
					return _string == rhs._string;
				}
				bool operator<(const StringWithComparator& rhs) const
				{
					return ::strcmp(_string.c_str(), rhs._string.c_str()) < 0;
				}
				bool operator>(const StringWithComparator& rhs) const
				{
					return ::strcmp(_string.c_str(), rhs._string.c_str()) > 0;
				}
			};
			Vector<StringWithComparator> fileNames;
			fileNames.push_back(StringWithComparator("images\\game\\map\\forest\\forest1\\tile\\t_dg_01.pgf"));
			fileNames.push_back(StringWithComparator("images\\game\\map\\forest\\forest1\\tile\\t_dg_02.pgf"));
			fileNames.push_back(StringWithComparator("images\\game\\map\\forest\\forest1\\tile\\t_dg_03.pgf"));
			fileNames.push_back(StringWithComparator("images\\game\\map\\forest\\forest1\\tile\\t_dg_04.pgf"));
			MINT_ASSURE(binarySearch(fileNames, StringWithComparator("images\\game\\map\\forest\\forest1\\tile\\t_dg_03.bmp")) < 0);
			return true;
		}
	}
}
