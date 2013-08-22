#include <corelib/IPlugin.h>
#include <plugins/dfelements/DFProcessors.h>
#include <plugins/dfelements/DfSinks.h>
#include <plugins/dfelements/DFSources.h>
#include <plugins/newVdf/IDataFlowProvider.h>

CORE_PLUGIN_BEGIN("DFEments", core::UID::GenerateUniqueID("{678727A0-ED31-43CA-AE6D-BADA05717ADA}"))
	VDF_SERVICE_BEGIN(DFElementsService, "{C354FD3F-3559-4990-830D-57BA5E5BC813}")

        VDF_ADD_DATA_SOURCE(
            UniversalSource<VectorChannelReaderInterface>, 
            "Vector source",
            "{03FCCADA-5814-4B7B-BC60-2E07CBBF35FB}",
            QIcon(":/dfElements/icons/source.png"));

		VDF_ADD_DATA_PROCESSOR(
			ScalarTreshold, 
			"Treshold",
			"{1143D447-2F76-458A-ADEB-4A669E487023}",
			QIcon(":/dfElements/icons/processor.png"));

		VDF_ADD_DATA_PROCESSOR(
			ScalarInverter, 
			"Inverter",
			"{FD7BAFCA-13D9-47E4-B080-B151733F5255}",
			QIcon(":/dfElements/icons/invert.png"));

		VDF_ADD_DATA_PROCESSOR(
			ScalarMul, 
			"Product",
			"{18C4C196-3CC5-4217-B8C4-DDDE431BCDE6}",
			QIcon(":/dfElements/icons/processor.png"));

		VDF_ADD_DATA_PROCESSOR(
			ScalarAdd, 
			"Sum",
			"{3E04C29A-9678-47B9-A835-B0C167D4CE3A}",
			QIcon(":/dfElements/icons/sum.png"));

		VDF_ADD_DATA_PROCESSOR(
			ScalarDiff, 
			"Difference",
			"{298C184E-D459-4D60-B8A3-5BB694E905FE}",
			QIcon(":/dfElements/icons/minusplus.png"));

		VDF_ADD_DATA_PROCESSOR(
			VectorDiff, 
			"Difference(Vector)",
			"{A6AFDF0D-1467-492D-BA07-AD7CB1773103}",
			QIcon(":/dfElements/icons/minusplus.png"));

		VDF_ADD_DATA_PROCESSOR(
			Vector2Scalar, 
			"Vector->Scalar",
			"{703C8290-C7C7-4F4B-935B-7E8DBFB08DCC}",
			QIcon(":/dfElements/icons/processor.png"));

		VDF_ADD_DATA_PROCESSOR(
			Scalar2Vector, 
			"Scalar->Vector",
			"{F0D38B83-B470-480A-8C07-7A8B714E5E28}",
			QIcon(":/dfElements/icons/processor.png"));

		VDF_ADD_DATA_PROCESSOR(
			VectorAdder, 
			"Sum (Vector)",
			"{C47836AC-2B12-48AC-BB81-90B1DCA05CE3}",
			QIcon(":/dfElements/icons/sum.png"));

		VDF_ADD_DATA_PROCESSOR(
			ScalarDummy, 
			".High-pass",
			"{8E4E35A1-219B-4C06-A3B7-96BDC16D4638}",
			QIcon(":/dfElements/icons/processor.png"));

		VDF_ADD_DATA_PROCESSOR(
			ScalarDummy, 
			".Raised-cosine",
			"{6633ABE2-7874-4D48-A818-828A1BA306FB}",
			QIcon(":/dfElements/icons/processor.png"));

		VDF_ADD_DATA_PROCESSOR(
			ScalarDummy, 
			".Low-pass",
			"{023FFF19-DC7E-4D73-B05F-DBEF48AF756C}",
			QIcon(":/dfElements/icons/processor.png"));

		VDF_ADD_DATA_PROCESSOR(
			ScalarDummy, 
			".Gaussian",
			"{1C685D1F-179B-4EFF-A74E-38FD527F079C}",
			QIcon(":/dfElements/icons/processor.png"));

		VDF_ADD_DATA_PROCESSOR(
			ScalarDummy, 
			".Elliptic",
			"{45FD6E59-3D6D-4B72-A15C-2C0A9AB9E45A}",
			QIcon(":/dfElements/icons/processor.png"));

		VDF_ADD_DATA_PROCESSOR(
			ScalarDummy, 
			".Bessel",
			"{1543D0DA-9009-41E4-A9B6-A88A06A51C17}",
			QIcon(":/dfElements/icons/processor.png"));

		VDF_ADD_DATA_PROCESSOR(
			ScalarDummy, 
			".Chebyshev (I)",
			"{E3FA3626-7520-4275-86FD-98635BC66CE7}",
			QIcon(":/dfElements/icons/processor.png"));

		VDF_ADD_DATA_PROCESSOR(
			ScalarDummy, 
			".Chebyshev (II)",
			"{B4F8D519-E245-4203-A02B-906211788FAE}",
			QIcon(":/dfElements/icons/processor.png"));


		VDF_ADD_DATA_PROCESSOR(
			ScalarDummy, 
			".Time Normalization",
			"{E035F89F-4529-4CE1-83D7-6A2EAEBFFEA8}",
			QIcon(":/dfElements/icons/normalizeH.png"));


		VDF_ADD_DATA_PROCESSOR(
			ScalarDummy, 
			".Normalization",
			"{3B6D7C28-C681-4A76-B1F9-77AA58AA4899}",
			QIcon(":/dfElements/icons/normalizeV.png"));

		VDF_ADD_DATA_PROCESSOR(
			ScalarDummy, 
			".FFT",
			"{2964EC49-BAA2-4048-A67C-BE1B5D56A580}",
			QIcon(":/dfElements/icons/processor.png"));

		VDF_ADD_DATA_PROCESSOR(
			ScalarDummy, 
			".Smooth",
			"{0EE64473-2E58-4277-9E83-4B735FE2D751}",
			QIcon(":/dfElements/icons/processor.png"));

        VDF_ADD_DATA_SINK(
            UniversalSink,
            "Tree Sink",
            "{92C7B33D-9AEA-46AE-BEBD-E0EF760C5038}",
            QIcon(":/dfElements/icons/sink.png"));

	VDF_SERVICE_END(DFElementsService)
CORE_PLUGIN_END
