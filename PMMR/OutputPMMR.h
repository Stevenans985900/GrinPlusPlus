#pragma once

#include "Common/MMR.h"
#include "Common/CRoaring/roaring.hh"
#include "Common/LeafSet.h"
#include "Common/PruneList.h"
#include "Common/HashFile.h"
#include "Common/DataFile.h"

#include <Core/OutputIdentifier.h>
#include <Database/BlockDB.h>
#include <Config/Config.h>
#include <Hash.h>

#define OUTPUT_SIZE 34

class OutputPMMR : public MMR
{
public:
	static OutputPMMR* Load(const Config& config, IBlockDB& blockDB);
	~OutputPMMR();

	bool Append(const OutputIdentifier& output, const uint64_t blockHeight);
	bool Remove(const uint64_t mmrIndex);

	virtual Hash Root(const uint64_t mmrIndex) const override final;
	virtual std::unique_ptr<Hash> GetHashAt(const uint64_t mmrIndex) const override final;
	virtual uint64_t GetSize() const override final;

	virtual bool Rewind(const uint64_t size) override final;
	virtual bool Flush() override final;
	virtual bool Discard() override final;

	std::unique_ptr<OutputIdentifier> GetOutputAt(const uint64_t mmrIndex) const;

private:
	OutputPMMR(const Config& config, IBlockDB& blockDB, HashFile* pHashFile, LeafSet&& leafSet, PruneList&& pruneList, DataFile<OUTPUT_SIZE>* pDataFile);

	Roaring DetermineLeavesToRemove(const uint64_t cutoffSize, const Roaring& rewindRmPos) const;
	Roaring DetermineNodesToRemove(const Roaring& leavesToRemove) const;

	const Config& m_config;
	IBlockDB& m_blockDB;

	HashFile* m_pHashFile;
	LeafSet m_leafSet;
	PruneList m_pruneList;
	DataFile<OUTPUT_SIZE>* m_pDataFile;
};