#include "CSE/Base/AdvMath.h"

_CSE_BEGIN
_SCICXX_BEGIN

void cse::SciCxx::ElementwiseIterator::Run(Function1D Function, float64 MaxIterLog)
{
    uint64 MaxIter = uint64(floor(pow(10, MaxIterLog)));
    bool IsActive = !CheckTerminate();
    while (IterCount < MaxIter && IsActive)
    {
        DynamicMatrix<float64> x = PreEvaluator();
        DynamicMatrix<float64> fx(x);
        for (std::size_t col = 0; col < x.col(); ++col)
        {
            for (std::size_t row = 0; row < x.row(); ++row)
            {
                fx.at(col, row) = Function(x.at(col, row));
            }
        }
        EvaluateCount += x.row();
        PostEvaluator(x, fx);
        ++IterCount;
        IsActive = !CheckTerminate();
        if (!IsActive) {break;}
        Finalize();
    }
}

_SCICXX_END
_CSE_END
