/*
 * PROPRIETARY INFORMATION.  This software is proprietary to
 * Side Effects Software Inc., and is not to be reproduced,
 * transmitted, or disclosed in any way without written permission.
 *
 * Produced by:
 *      Mykola Konyk
 *      Side Effects Software Inc
 *      123 Front Street West, Suite 1401
 *      Toronto, Ontario
 *      Canada   M5J 2M2
 *      416-504-9876
 *
 */

#pragma once
#include "HoudiniGeoPartObject.h"
#include "HoudiniAssetInstanceInput.h"
#include "HoudiniAssetInstanceInputField.generated.h"

class UStaticMesh;
class UMaterialInterface;
class UHoudiniAssetComponent;
struct FHoudiniGeoPartObject;

UCLASS()
class HOUDINIENGINERUNTIME_API UHoudiniAssetInstanceInputField : public UObject
{
    friend class UHoudiniAssetInstanceInput;

    GENERATED_UCLASS_BODY()

    public:

        /** Create an instance of input field. **/
        static UHoudiniAssetInstanceInputField * Create(
            UHoudiniAssetComponent * InHoudiniAssetComponent,
            UHoudiniAssetInstanceInput * InHoudiniAssetInstanceInput,
            const FHoudiniGeoPartObject & HoudiniGeoPartObject );

        /** Create an instance of input field from another input field. **/
        static UHoudiniAssetInstanceInputField * Create(
            UHoudiniAssetComponent * InHoudiniAssetComponent,
            const UHoudiniAssetInstanceInputField * OtherInputField );

    /** UObject methods. **/
    public:

        virtual void Serialize( FArchive & Ar ) override;
        static void AddReferencedObjects( UObject * InThis, FReferenceCollector & Collector );
        virtual void BeginDestroy() override;

#if WITH_EDITOR

        virtual void PostEditUndo() override;

#endif

    public:

        /** Return geo part object associated with this field. **/
        const FHoudiniGeoPartObject & GetHoudiniGeoPartObject() const;

        /** Refresh state based on the given geo part object */
        void SetGeoPartObject( const FHoudiniGeoPartObject & InHoudiniGeoPartObject );

        /** Return original static mesh. **/
        UObject* GetOriginalObject() const;

        /** Return the static mesh associated with an instance variation. **/
        UObject * GetInstanceVariation( int32 VariationIndex ) const;  

        /** Add a variation to the instancing. **/
        void AddInstanceVariation( UObject * InstancedObject, int32 VariationIdx );

        /** Replace the instance variation in a particular slot. **/
        void ReplaceInstanceVariation( UObject * InObject, int32 Index );

        /** Remove a variation from instancing **/
        void RemoveInstanceVariation( int32 VariationIdx );

        /** Returns the number of instance variations. **/
        int32 InstanceVariationCount() const;

        /** Given a static mesh, find which slot(s) it occupies in the instance variations. **/
        void FindObjectIndices( UObject * InStaticMesh, TArray< int32 > & Indices );

        /** Get material replacements. **/
        bool GetMaterialReplacementMeshes(
            UMaterialInterface * Material,
            TMap< UStaticMesh *, int32 > & MaterialReplacementsMap );

#if WITH_EDITOR

        /** Set thumbnail border used by static mesh used by this field. **/
        void AssignThumbnailBorder( TSharedPtr< SBorder > InThumbnailBorder );

        /** Get thumbnail border used by static mesh used by this field. **/
        TSharedPtr< SBorder > GetThumbnailBorder() const;

        /** Set combo button used by static mesh used by this field. **/
        void AssignComboButton( TSharedPtr< SComboButton > InComboButton );

        /** Get combo button used by static mesh used by this field. **/
        TSharedPtr< SComboButton > GetComboButton() const;

#endif

        /** Get rotator used by this field. **/
        const FRotator & GetRotationOffset( int32 VariationIdx ) const;

        /** Set rotation offset used by this field. **/
        void SetRotationOffset( const FRotator & Rotator, int32 VariationIdx );

        /** Get scale used by this field. **/
        const FVector & GetScaleOffset( int32 VariationIdx ) const;

        /** Set scale used by this field. **/
        void SetScaleOffset( const FVector & InScale, int32 VariationIdx );

        /** Return true if all fields are scaled linearly. **/
        bool AreOffsetsScaledLinearly( int32 VariationIdx ) const;

        /** Set whether offsets are scaled linearly. **/
        void SetLinearOffsetScale( bool bEnabled, int32 VariationIdx );

        /** Return true if original static mesh is used. **/
        bool IsOriginalObjectUsed( int32 VariationIdx ) const;

        /** Return corresponding instanced static mesh component. **/
        class USceneComponent * GetInstancedComponent( int32 VariationIdx ) const;

        /** Return transformations of all instances used by the variation **/
        const TArray< FTransform > & GetInstancedTransforms( int32 VariationIdx ) const;

        /** Return the array of transforms for all variations **/
        FORCEINLINE const TArray< FTransform > & GetInstancedTransforms() { return InstancedTransforms; }

        /** Recreates render states for instanced static mesh component. **/
        void RecreateRenderState();

        /** Recreates physics states for instanced static mesh component. **/
        void RecreatePhysicsState();

    protected:

        /** Create instanced component for this field. **/
        void AddInstanceComponent( int32 VariationIdx );

        /** Set transforms for this field. **/
        void SetInstanceTransforms( const TArray< FTransform > & ObjectTransforms );

        /** Update relative transform for this field. **/
        void UpdateRelativeTransform();

        /** Update instance transformations. **/
        void UpdateInstanceTransforms( bool RecomputeVariationAssignments );

    protected:

#if WITH_EDITOR

        /** Thumbnail border used by slate for this field. **/
        TSharedPtr< SBorder > ThumbnailBorder;

        /** Combo box element used by slate for this field. **/
        TSharedPtr< SComboButton > StaticMeshComboButton;

#endif

        /** Original object used by the instancer. **/
        UObject* OriginalObject;
        
        /** Currently used Objects */
        TArray< UObject* > InstancedObjects;
        
        /** Used instanced actor component. **/
        TArray< USceneComponent * > InstancerComponents;

        /** Parent Houdini asset component. **/
        UHoudiniAssetComponent * HoudiniAssetComponent;

        /** Owner input. **/
        UHoudiniAssetInstanceInput * HoudiniAssetInstanceInput;

        /** Transforms, one for each instance. **/
        TArray< FTransform > InstancedTransforms;

        /** Assignment of Transforms to each variation **/
        TArray< TArray< FTransform > > VariationTransformsArray;

        /** Corresponding geo part object. **/
        FHoudiniGeoPartObject HoudiniGeoPartObject;

        /** Rotation offset for instanced component. **/
        TArray< FRotator > RotationOffsets;

        /** Scale offset for instanced component. **/
        TArray< FVector > ScaleOffsets;

        /** Whether to scale linearly for all fields. **/
        TArray< bool > bScaleOffsetsLinearlyArray;

        /** Flags used by this input field. **/
        uint32 HoudiniAssetInstanceInputFieldFlagsPacked;
};

/** Functor used to sort instance input fields. **/
struct HOUDINIENGINERUNTIME_API FHoudiniAssetInstanceInputFieldSortPredicate
{
    bool operator()( const UHoudiniAssetInstanceInputField & A, const UHoudiniAssetInstanceInputField & B ) const;
};
