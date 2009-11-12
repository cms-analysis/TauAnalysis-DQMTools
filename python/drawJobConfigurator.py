import FWCore.ParameterSet.Config as cms
import copy
import sys

#--------------------------------------------------------------------------------
# utility function for generation of drawJob configurations
# for DQMHistPlotter tool
#
# Author: Christian Veelken, UC Davis
#
#--------------------------------------------------------------------------------

class drawJobConfigurator(cms._ParameterTypeBase):

    def __init__(self, template, dqmDirectory):
        self.template = template
        self.dqmDirectory = dqmDirectory

        if not self.dqmDirectory.endswith("/"):
            self.dqmDirectory += "/"

        self.drawJobs = cms.PSet()

    @staticmethod
    def _getCutName(cut):
        if ( isinstance(cut, cms.PSet) ):
            return getattr(cut, "pluginName").value()
        elif ( isinstance(cut, cms.string) ):
            return cut.value()
        elif ( isinstance(cut, str) ):            
            return cut
        else:
            raise ValueError("Invalid type for 'cut' Parameter !!")

    @staticmethod
    def _composeSubDirectoryName(afterCut = None, beforeCut = None):
        # auxiliary function to compose name of dqmSubDirectory
        # in which histograms filled after applying afterCut,
        # but before applying beforeCut, are stored

        dqmSubDirectory = ""
        if ( afterCut is not None ):
            afterCut_name = drawJobConfigurator._getCutName(afterCut)
            dqmSubDirectory += "after" + afterCut_name[0:1].capitalize() + afterCut_name[1:]
        if ( beforeCut is not None ):
            beforeCut_name = drawJobConfigurator._getCutName(beforeCut)
            if dqmSubDirectory != "":
                dqmSubDirectory += "_"
            dqmSubDirectory += "before" + beforeCut_name[0:1].capitalize() + beforeCut_name[1:]

        return dqmSubDirectory;

    def add(self, afterCut = None, beforeCut = None, plot = None, plots = None):
        # configure drawJob
        # and add to configuration object

        # check validity of parameters passed as function arguments
        if self.template is None:
            raise ValueError("Invalid 'template' Parameter !!")
        if plot is None and plots is None:
            raise ValueError("Invalid 'plot' and 'plots' Parameters !!")
        if afterCut is None and beforeCut is None:
            raise ValueError("Invalid 'afterCuts' and 'beforeCuts' Parameters !!")

        # check if need to call recursively
        # in case of multiple plots
        if plots is not None:
            for plot in plots:
                self.add(afterCut = afterCut, beforeCut = beforeCut, plot = plot)
            return

        drawJob = copy.deepcopy(self.template)

        dqmDirectory = self.dqmDirectory
        dqmDirectory += drawJobConfigurator._composeSubDirectoryName(afterCut = afterCut, beforeCut = beforeCut)
        if not dqmDirectory.endswith("/"):
            dqmDirectory += "/"

        dqmMonitorElement = dqmDirectory + getattr(plot, "meName")

        setattr(drawJob.plots, "dqmMonitorElements", cms.vstring([ dqmMonitorElement, ]))
        if hasattr(plot, "PAR"):
            setattr(drawJob, "parameter", cms.vstring(getattr(plot, "PAR")))
        setattr(drawJob, "title", cms.string(getattr(plot, "title")))
        setattr(drawJob, "xAxis", cms.string(getattr(plot, "xAxis")))

        # add drawJob configuration to set of drawJobs
        setattr(self.drawJobs, getattr(plot, "name"), drawJob)

    def configure(self):
        # return configuration object
        # for set of drawJobs

        return self.drawJobs

#--------------------------------------------------------------------------------
# auxiliary class stroring configuration parameters for one drawJob
#--------------------------------------------------------------------------------

class drawJobConfigEntry(cms._ParameterTypeBase):

    def __init__(self, meName, title, xAxis, name, PAR = None):
        self.meName = meName
        self.title = title
        self.xAxis = xAxis
        self.name = name
        if PAR is not None:
            self.PAR = PAR
