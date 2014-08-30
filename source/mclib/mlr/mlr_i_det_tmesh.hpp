//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_DeT_TMESH_HPP
#define MLR_MLR_I_DeT_TMESH_HPP

#include <mlr/gosvertex.hpp>
#include <mlr/mlr_i_tmesh.hpp>
#include <mlr/mlr_i_det_pmesh.hpp>

extern uint32_t gEnableDetailTexture;

namespace MidLevelRenderer {

	class MLR_I_DeT_PMesh;

	//##########################################################################
	//#### MLRIndexedPolyMesh with no color no lighting w/ detail texture  #####
	//##########################################################################

	class MLR_I_DeT_TMesh:
		public MLR_I_TMesh
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Initialization
		//
	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors/Destructors
		//
	protected:
		MLR_I_DeT_TMesh(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			uint32_t version
			);
		~MLR_I_DeT_TMesh(void);

	public:
		MLR_I_DeT_TMesh(ClassData *class_data=MLR_I_DeT_TMesh::DefaultData);

		static MLR_I_DeT_TMesh*
			Make(
			Stuff::MemoryStream *stream,
			uint32_t version
			);

		void
			Save(Stuff::MemoryStream *stream);

	public:
		void
			Copy(
			MLR_I_TMesh*,
			MLRState detailState,
			float xOff,
			float yOff,
			float xFac,
			float yFac
			);

		bool Copy(MLR_I_DeT_PMesh *pmesh)
		{
			Check_Object(pmesh);
#ifdef _GAMEOS_HPP_
			STOP(("Not implemented yet !"));
#endif
			return false;
		}

		void
			SetDetailData(
			float xOff,
			float yOff,
			float xFac,
			float yFac,
			float detailStart,
			float detailEnd
			);

		void
			GetDetailData(
			float& xOff,
			float& yOff,
			float& xFac,
			float& yFac,
			float& dStart,
			float& dEnd
			)
		{
			Check_Object(this);
			xOff = xOffset;
			yOff = yOffset;
			xFac = xScale;
			yFac = yScale;
			dStart = fadeDetailStart;
			dEnd = fadeDetailEnd;
		}

		virtual int32_t	TransformAndClip(Stuff::Matrix4D *, MLRClippingState, GOSVertexPool*,bool=false);

		virtual void
			TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*,bool=false);

		virtual void SetReferenceState(const MLRState& _state, size_t pass=0)
		{
			Check_Object(this);
			Verify((intptr_t(pass) >= 0) && pass<2);
			if(pass==0)
			{
				referenceState = _state;
			}
			else
			{
				passes = 2;
				referenceState2 = _state;
			}
		}
		virtual const MLRState&
			GetReferenceState(size_t pass=0) const
		{
			Check_Object(this); 
			if(pass==0)
				return referenceState;
			else
				return referenceState2;
		}
		virtual const MLRState&
			GetCurrentState(size_t pass=0) const
		{
			Check_Object(this);
			if(pass==0)
				return state;
			else
				return state2;
		}

		virtual void
			CombineStates (const MLRState& master)
		{
			Check_Object(this);
			state.Combine(master, referenceState); 
			if(gEnableDetailTexture==0)
			{
				state.SetMultiTextureMode(MLRState::MultiTextureOffMode);
			}
			state2.Combine(master, referenceState2);
		};

		virtual GOSVertex* GetGOSVertices(uint32_t pass=0)
		{
			Check_Object(this); 
			if(pass==0)
				return gos_vertices;
			else
				return gos_vertices + numGOSVertices;
		}

		virtual uint32_t GetNumPasses(void);

		GOSVertex2UV* GetGOSVertices2UV(uint32_t=0)
		{ Check_Object(this); return gos_vertices2uv; }

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Class Data Support
		//
	public:
		static ClassData* DefaultData;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Testing
		//
	public:
		void TestInstance(void) const;

		virtual size_t GetSize(void)
		{ 
			Check_Object(this);
			size_t ret = MLR_I_TMesh::GetSize();

			return ret;
		}

	protected:
		bool	detTextureVisible;

		MLRState	state2, referenceState2;

		float fadeDetailStart, fadeDetailEnd, fadeMultiplicator;

		float xOffset, yOffset, xScale, yScale;

		GOSVertex2UV *gos_vertices2uv;
	};

	MLR_I_DeT_TMesh*
		CreateIndexedTriCube_NoColor_NoLit_DetTex(float, MLRState*, MLRState*);
	MLRShape*
		CreateIndexedTriIcosahedron_NoColor_NoLit_DetTex(
		IcoInfo&,
		MLRState*,
		MLRState*
		);

}
#endif
