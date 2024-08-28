/*=============================================================================
	UnEditorNative.h: Native function lookup table for static libraries.
	Copyright 2021 OldUnreal. All Rights Reserved.

	Revision history:
		* Created by Stijn Volckaert
=============================================================================*/

#ifndef UNEDITORNATIVE_H
#define UNEDITORNATIVE_H

DECLARE_NATIVE_TYPE(Editor, UBrushBuilder);
DECLARE_NATIVE_TYPE(Editor, UEdGUI_CheckBox);
DECLARE_NATIVE_TYPE(Editor, UEdGUI_Component);
DECLARE_NATIVE_TYPE(Editor, UEdGUI_Base);
DECLARE_NATIVE_TYPE(Editor, UEdGUI_ComboBox);
DECLARE_NATIVE_TYPE(Editor, UEdGUI_WindowFrame);

#define AUTO_INITIALIZE_REGISTRANTS_EDITOR			\
	UBatchExportCommandlet::StaticClass();			\
	UBrushBuilder::StaticClass();					\
	UConformCommandlet::StaticClass();				\
	UCheckUnicodeCommandlet::StaticClass();			\
	UPackageFlagCommandlet::StaticClass();			\
	UDataRipCommandlet::StaticClass();				\
	UStripSourceCommandlet::StaticClass();			\
	UDumpIntCommandlet::StaticClass();				\
	UListObjectsCommandlet::StaticClass();			\
	UMakeCommandlet::StaticClass();					\
	UMasterCommandlet::StaticClass();				\
	UUpdateUModCommandlet::StaticClass();			\
	UChecksumPackageCommandlet::StaticClass();		\
	UTextBufferExporterTXT::StaticClass();			\
	USoundExporterWAV::StaticClass();				\
	UMusicExporterTracker::StaticClass();			\
	UClassExporterH::StaticClass();					\
	UStrippedClassExporterUC::StaticClass();		\
	UClassExporterUC::StaticClass();				\
	UPolysExporterT3D::StaticClass();				\
	UModelExporterT3D::StaticClass();				\
	ULevelExporterT3D::StaticClass();				\
	ULevelFactoryNew::StaticClass();				\
	UClassFactoryNew::StaticClass();				\
	UTextureFactoryNew::StaticClass();				\
	UClassFactoryUC::StaticClass();					\
	ULevelFactory::StaticClass();					\
	UPolysFactory::StaticClass();					\
	UModelFactory::StaticClass();					\
	USoundFactory::StaticClass();					\
	UMusicFactory::StaticClass();					\
	UTextureExporterPCX::StaticClass();				\
	UTextureExporterBMP::StaticClass();				\
	UEditorEngine::StaticClass();					\
	UTransBuffer::StaticClass();					\
	UTransactor::StaticClass();						\
	/*	UBitArray::StaticClass();*/					\
	/*	UBitMatrix::StaticClass();*/				\
	UTextureFactory::StaticClass();					\
	UTrueTypeFontFactory::StaticClass();			\
	UFontExporter::StaticClass();					\
	URipAndTearCommandlet::StaticClass();			\
	UDumpMeshInfoCommandlet::StaticClass();			\
	USaveEmbeddedCommandlet::StaticClass();			\
	UEdCommandHook::StaticClass();					\
	UFontPageDiffCommandlet::StaticClass();			\
	UFontCompressCommandlet::StaticClass();			\
	UDumpTextureInfoCommandlet::StaticClass();		\
	UMusicPackagesCommandlet::StaticClass();		\
	URebuildImportsCommandlet::StaticClass();		\
	UTextureMergerCommandlet::StaticClass();		\
	UPolysExporterOBJ::StaticClass();				\
	UObjectExporterT3D::StaticClass();				\
	UFullBatchExportCommandlet::StaticClass();		\
	UProdigiosumInParvoCommandlet::StaticClass();	\
	UTextureExporterPNG::StaticClass();				\
	UTextureExporterDDS::StaticClass();				\
	UFontFactory::StaticClass();					\
	UMeshAnimExpTXT::StaticClass();					\
	UStaticMeshExpTXT::StaticClass();				\
	USkeletalMeshExpPSK::StaticClass();				\
	USkeletalAnimExpPSA::StaticClass();				\
	UBatchMeshExportCommandlet::StaticClass();		\
	UReduceTexturesCommandlet::StaticClass();		\
	UAudioPackageCommandlet::StaticClass();			\
	UPS2ConvertCommandlet::StaticClass();			\
	UEdGUI_Base::StaticClass();						\
	UEdGUI_WindowFrame::StaticClass();				\
	UEdGUI_Component::StaticClass();				\
	UEdGUI_Button::StaticClass();					\
	UEdGUI_Label::StaticClass();					\
	UEdGUI_EditBox::StaticClass();					\
	UEdGUI_ComboBox::StaticClass();					\
	UEdGUI_CheckBox::StaticClass();

#endif
