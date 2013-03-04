#include "hmmPCH.h"
#include "HmmVdf.h"
#include "HmmSources.h"
#include "HmmSinks.h"
#include "HmmPins.h"
#include "HmmProcessors.h"


void HmmVdf::registerNodes(HmmMainWindow* hmm)
{

    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarTreshold, 
        "Treshold",
        "{1143D447-2F76-458A-ADEB-4A669E487023}",
        QIcon(":/resources/icons/vdf/processor.png"));

    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarInverter, 
        "Inverter",
        "{FD7BAFCA-13D9-47E4-B080-B151733F5255}",
        QIcon(":/resources/icons/vdf/invert.png"));
    
    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarMul, 
        "Product",
        "{18C4C196-3CC5-4217-B8C4-DDDE431BCDE6}",
        QIcon(":/resources/icons/vdf/processor.png"));

    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarAdd, 
        "Sum",
        "{3E04C29A-9678-47B9-A835-B0C167D4CE3A}",
        QIcon(":/resources/icons/vdf/sum.png"));

    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarDiff, 
        "Difference",
        "{298C184E-D459-4D60-B8A3-5BB694E905FE}",
        QIcon(":/resources/icons/vdf/minusplus.png"));

    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        VectorDiff, 
        "Difference(Vector)",
        "{A6AFDF0D-1467-492D-BA07-AD7CB1773103}",
        QIcon(":/resources/icons/vdf/minusplus.png"));

    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        Vector2Scalar, 
        "Vector->Scalar",
        "{703C8290-C7C7-4F4B-935B-7E8DBFB08DCC}",
        QIcon(":/resources/icons/vdf/processor.png"));

    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        Scalar2Vector, 
        "Scalar->Vector",
        "{F0D38B83-B470-480A-8C07-7A8B714E5E28}",
        QIcon(":/resources/icons/vdf/processor.png"));
    
    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        VectorAdder, 
        "Sum (Vector)",
        "{C47836AC-2B12-48AC-BB81-90B1DCA05CE3}",
        QIcon(":/resources/icons/vdf/sum.png"));

    HMM_REGISTER_VECTOR_DATA_SOURCE(
        Forces,
        "Forces",
        "{0300FAAE-056A-451E-AF2D-3EF3EA00D3F6}",
        QIcon(":/resources/icons/vdf/force.png"), 
        hmm);

    HMM_REGISTER_VECTOR_DATA_SOURCE(
        Powers,
        "Powers",
        "{B868ABAB-8F23-4383-8649-4B467527D9B1}",
        QIcon(":/resources/icons/vdf/power.png"), 
        hmm);

    HMM_REGISTER_VECTOR_DATA_SOURCE(
        Moments,
        "Moments",
        "{06533E53-98A8-46B1-B768-051B374C355D}",
        QIcon(":/resources/icons/vdf/moment.png"), 
        hmm);

    HMM_REGISTER_VECTOR_DATA_SOURCE(
        Markers,
        "Markers",
        "{13186456-BBAA-4AC0-A42F-75CC7717395D}",
        QIcon(":/resources/icons/vdf/marker.png"), 
        hmm);

    HMM_REGISTER_VECTOR_DATA_SOURCE(
        Joints,
        "Joints",
        "{A23986A8-147D-4A53-9559-33ACBA4CE174}",
        QIcon(":/resources/icons/vdf/joint.png"), 
        hmm);

    HMM_REGISTER_VECTOR_DATA_SOURCE(
        GRF,
        "GRF",
        "{FC67A0E3-FED2-44EA-9F61-4E492AF12E2D}",
        QIcon(":/resources/icons/vdf/grf.png"), 
        hmm);

    auto xsink = new XSink(hmm);
    auto xlambda = 
    [&](const df::ISinkNode* prototype) -> XSink*
    { 
        auto sink = new XSink();
        auto proto = dynamic_cast<const XSink*>(prototype);
        sink->setHmm(proto->getHmm());
        return sink; 
    };
    HMM_REGISTER_DATA_SINK(
        "Result sink", 
        "{57A82EDD-E890-4A6E-8DA9-03CF74F91E32}",
        QIcon(":/resources/icons/vdf/sink.png"), 
        xsink, 
        xlambda);

    auto emgsource = new EMGSource(hmm);
    auto emgLambda = 
        [&](const df::ISourceNode* prototype) -> EMGSource*
        { 
            auto proto = dynamic_cast<const EMGSource*>(prototype);
            auto source = new EMGSource(proto->getHmm());
            return source; 
        };
    HMM_REGISTER_DATA_SOURCE(
        "EMG (raw)", 
        "{98EFFA9C-7A7F-4561-96D8-9DCE47136AE3}",
        QIcon(":/resources/icons/vdf/emg.png"), 
        emgsource, 
        emgLambda);

    // ------------------------ !!!! FAKE !!!!--------------------------------------------
    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarDummy, 
        ".High-pass",
        "{8E4E35A1-219B-4C06-A3B7-96BDC16D4638}",
        QIcon(":/resources/icons/vdf/processor.png"));

    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarDummy, 
        ".Raised-cosine",
        "{6633ABE2-7874-4D48-A818-828A1BA306FB}",
        QIcon(":/resources/icons/vdf/processor.png"));

    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarDummy, 
        ".Low-pass",
        "{023FFF19-DC7E-4D73-B05F-DBEF48AF756C}",
        QIcon(":/resources/icons/vdf/processor.png"));

    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarDummy, 
        ".Gaussian",
        "{1C685D1F-179B-4EFF-A74E-38FD527F079C}",
        QIcon(":/resources/icons/vdf/processor.png"));

    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarDummy, 
        ".Elliptic",
        "{45FD6E59-3D6D-4B72-A15C-2C0A9AB9E45A}",
        QIcon(":/resources/icons/vdf/processor.png"));

    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarDummy, 
        ".Bessel",
        "{1543D0DA-9009-41E4-A9B6-A88A06A51C17}",
        QIcon(":/resources/icons/vdf/processor.png"));

    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarDummy, 
        ".Chebyshev (I)",
        "{E3FA3626-7520-4275-86FD-98635BC66CE7}",
        QIcon(":/resources/icons/vdf/processor.png"));

    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarDummy, 
        ".Chebyshev (II)",
        "{B4F8D519-E245-4203-A02B-906211788FAE}",
        QIcon(":/resources/icons/vdf/processor.png"));


    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarDummy, 
        ".Time Normalization",
        "{E035F89F-4529-4CE1-83D7-6A2EAEBFFEA8}",
        QIcon(":/resources/icons/vdf/normalizeH.png"));


    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarDummy, 
        ".Normalization",
        "{3B6D7C28-C681-4A76-B1F9-77AA58AA4899}",
        QIcon(":/resources/icons/vdf/normalizeV.png"));

    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarDummy, 
        ".FFT",
        "{2964EC49-BAA2-4048-A67C-BE1B5D56A580}",
        QIcon(":/resources/icons/vdf/processor.png"));

    HMM_REGISTER_DATA_PROCESSOR_SIMPLE(
        ScalarDummy, 
        ".Smooth",
        "{0EE64473-2E58-4277-9E83-4B735FE2D751}",
        QIcon(":/resources/icons/vdf/processor.png"));


    // ------------------------ !!!! FAKE END !!!!--------------------------------------------
}