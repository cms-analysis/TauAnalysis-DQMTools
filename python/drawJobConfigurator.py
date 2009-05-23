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
        if ( afterCut is not None ):
            afterCut_name = getattr(afterCut, "pluginName").value()
            dqmDirectory += "after" + afterCut_name[0:1].capitalize() + afterCut_name[1:]
        if ( beforeCut is not None ):
            beforeCut_name = getattr(afterCut, "pluginName").value()
            if not dqmDirectory.endswith("/"):
                dqmDirectory += "_"
            dqmDirectory += "before" + beforeCut_name[0:1].capitalize() + beforeCut_name[1:]
        if not dqmDirectory.endswith("/"):
            dqmDirectory += "/"    

        setattr(drawJob.plots, "dqmMonitorElements", cms.vstring(dqmDirectory + getattr(plot, "meName")))
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
