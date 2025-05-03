#include <MintLibrary/Include/Test.h>

#include <MintLibrary/Include/AllHeaders.h>
#include <MintLibrary/Include/AllHpps.h>


namespace mint
{
	namespace TestLibrary
	{
		bool Test()
		{
			MINT_ASSURE(Test_Algorithm());
			return true;
		}

		bool Test_Algorithm()
		{
			using namespace mint;

			Vector<uint32> t0;
			t0.PushBack(7);
			t0.PushBack(9);
			t0.PushBack(8);
			t0.PushBack(6);
			t0.PushBack(4);
			QuickSort(t0, ComparatorAscending<uint32>());
			MINT_ASSURE(t0[0] == 4);
			MINT_ASSURE(t0[1] == 6);
			MINT_ASSURE(t0[2] == 7);
			MINT_ASSURE(t0[3] == 8);
			MINT_ASSURE(t0[4] == 9);
			QuickSort(t0, ComparatorDescending<uint32>());
			MINT_ASSURE(t0[0] == 9);
			MINT_ASSURE(t0[1] == 8);
			MINT_ASSURE(t0[2] == 7);
			MINT_ASSURE(t0[3] == 6);
			MINT_ASSURE(t0[4] == 4);

			Vector<uint32> t1;
			t1.PushBack(3);
			t1.PushBack(2);
			t1.PushBack(1);
			t1.PushBack(4);
			t1.PushBack(5);
			QuickSort(t1, ComparatorAscending<uint32>());
			MINT_ASSURE(t1[0] == 1);
			MINT_ASSURE(t1[1] == 2);
			MINT_ASSURE(t1[2] == 3);
			MINT_ASSURE(t1[3] == 4);
			MINT_ASSURE(t1[4] == 5);
			QuickSort(t1, ComparatorDescending<uint32>());
			MINT_ASSURE(t1[0] == 5);
			MINT_ASSURE(t1[1] == 4);
			MINT_ASSURE(t1[2] == 3);
			MINT_ASSURE(t1[3] == 2);
			MINT_ASSURE(t1[4] == 1);

			Vector<uint32> t2;
			t2.PushBack(3);
			t2.PushBack(2);
			t2.PushBack(1);
			t2.PushBack(6);
			t2.PushBack(7);
			t2.PushBack(5);
			QuickSort(t2, ComparatorAscending<uint32>());
			MINT_ASSURE(t2[0] == 1);
			MINT_ASSURE(t2[1] == 2);
			MINT_ASSURE(t2[2] == 3);
			MINT_ASSURE(t2[3] == 5);
			MINT_ASSURE(t2[4] == 6);
			MINT_ASSURE(t2[5] == 7);
			QuickSort(t2, ComparatorDescending<uint32>());
			MINT_ASSURE(t2[0] == 7);
			MINT_ASSURE(t2[1] == 6);
			MINT_ASSURE(t2[2] == 5);
			MINT_ASSURE(t2[3] == 3);
			MINT_ASSURE(t2[4] == 2);
			MINT_ASSURE(t2[5] == 1);

			Vector<uint32> t3;
			t3.PushBack(1);
			t3.PushBack(2);
			t3.PushBack(3);
			t3.PushBack(4);
			t3.PushBack(5);
			QuickSort(t3, ComparatorAscending<uint32>());
			MINT_ASSURE(t3[0] == 1);
			MINT_ASSURE(t3[1] == 2);
			MINT_ASSURE(t3[2] == 3);
			MINT_ASSURE(t3[3] == 4);
			MINT_ASSURE(t3[4] == 5);
			QuickSort(t3, ComparatorDescending<uint32>());
			MINT_ASSURE(t3[0] == 5);
			MINT_ASSURE(t3[1] == 4);
			MINT_ASSURE(t3[2] == 3);
			MINT_ASSURE(t3[3] == 2);
			MINT_ASSURE(t3[4] == 1);

			Vector<uint32> t4;
			t4.PushBack(4);
			t4.PushBack(3);
			t4.PushBack(5);
			t4.PushBack(7);
			t4.PushBack(6);
			t4.PushBack(5);
			QuickSort(t4, ComparatorAscending<uint32>());
			MINT_ASSURE(t4[0] == 3);
			MINT_ASSURE(t4[1] == 4);
			MINT_ASSURE(t4[2] == 5);
			MINT_ASSURE(t4[3] == 5);
			MINT_ASSURE(t4[4] == 6);
			MINT_ASSURE(t4[5] == 7);
			QuickSort(t4, ComparatorDescending<uint32>());
			MINT_ASSURE(t4[0] == 7);
			MINT_ASSURE(t4[1] == 6);
			MINT_ASSURE(t4[2] == 5);
			MINT_ASSURE(t4[3] == 5);
			MINT_ASSURE(t4[4] == 4);
			MINT_ASSURE(t4[5] == 3);

			Vector<uint32> t5;
			t5.PushBack(0);
			t5.PushBack(0);
			t5.PushBack(0);
			QuickSort(t5, ComparatorAscending<uint32>());
			MINT_ASSURE(t5[0] == 0);
			MINT_ASSURE(t5[1] == 0);
			MINT_ASSURE(t5[2] == 0);
			QuickSort(t5, ComparatorDescending<uint32>());
			MINT_ASSURE(t5[0] == 0);
			MINT_ASSURE(t5[1] == 0);
			MINT_ASSURE(t5[2] == 0);

			Vector<uint32> t6;
			t6.PushBack(0);
			t6.PushBack(0);
			t6.PushBack(1);
			t6.PushBack(0);
			t6.PushBack(0);
			QuickSort(t6, ComparatorAscending<uint32>());
			MINT_ASSURE(t6[0] == 0);
			MINT_ASSURE(t6[1] == 0);
			MINT_ASSURE(t6[2] == 0);
			MINT_ASSURE(t6[3] == 0);
			MINT_ASSURE(t6[4] == 1);
			QuickSort(t6, ComparatorDescending<uint32>());
			MINT_ASSURE(t6[0] == 1);
			MINT_ASSURE(t6[1] == 0);
			MINT_ASSURE(t6[2] == 0);
			MINT_ASSURE(t6[3] == 0);
			MINT_ASSURE(t6[4] == 0);

			struct StringWithComparator
			{
				StringWithComparator(const StringReferenceA& string) : _string{ string.CString() } { __noop; }

				StackStringA<kMaxPath> _string;
				bool operator==(const StringWithComparator& rhs) const
				{
					return _string == rhs._string;
				}
				bool operator<(const StringWithComparator& rhs) const
				{
					return ::strcmp(_string.CString(), rhs._string.CString()) < 0;
				}
				bool operator>(const StringWithComparator& rhs) const
				{
					return ::strcmp(_string.CString(), rhs._string.CString()) > 0;
				}
			};
			Vector<StringWithComparator> fileNames;
			fileNames.PushBack(StringWithComparator("images\\game\\map\\forest\\forest1\\tile\\t_dg_01.pgf"));
			fileNames.PushBack(StringWithComparator("images\\game\\map\\forest\\forest1\\tile\\t_dg_02.pgf"));
			fileNames.PushBack(StringWithComparator("images\\game\\map\\forest\\forest1\\tile\\t_dg_03.pgf"));
			fileNames.PushBack(StringWithComparator("images\\game\\map\\forest\\forest1\\tile\\t_dg_04.pgf"));
			MINT_ASSURE(IsValidIndex(BinarySearch(fileNames, StringWithComparator("images\\game\\map\\forest\\forest1\\tile\\t_dg_03.bmp"))) == false);
			return true;
		}
	}
}
