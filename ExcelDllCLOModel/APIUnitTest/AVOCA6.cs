﻿using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using ManagedCLO;
using System.IO;
using System.Threading;
namespace APIUnitTest
{
    [TestClass]
    public class AVOCA6
    {

        [TestMethod]
        /// <summary>
        /// This test runs the model for the AVOCA VI deal as of 7-March-2014
        /// </summary>
        public void SimulateAvoca()
        {
            //Set up general parameters
            DateTime TempDate = new DateTime(2014, 7, 16);
            ManWaterfall AvocaStructure = new ManWaterfall();
            AvocaStructure.FirstIPDdate = TempDate;
            TempDate = new DateTime(2014, 1, 16);
            AvocaStructure.LastIPDdate = TempDate;
            AvocaStructure.PrincipalAvailable = 7530906.87;
            AvocaStructure.InterestAvailable = 1383098.59;
            AvocaStructure.CCCcurve = "5";
            AvocaStructure.CCChaircut = 0.5;
            AvocaStructure.CallMultiple = 1.0;
            AvocaStructure.CallReserve = 3;
            AvocaStructure.PoolValueAtCall = 100.0;
            TempDate = new DateTime(2013, 1, 16);
            AvocaStructure.SetupReinvestmentTest(TempDate, 1.036, 0.5, 0, 0, 0);
            AvocaStructure.SetupReinvBond("2.85", "20", "2", "50", "6 73R 0", 3, "N", 0.0038);
            AvocaStructure.CCCTestLimit = 0.05;
            AvocaStructure.SeniorExpenses = 0.0003;
            AvocaStructure.SeniorFees = 0.0015;
            AvocaStructure.JuniorFees = 0.005;
            AvocaStructure.JuniorFeesCoupon = 0.05;
            AvocaStructure.UseTurbo = true;
            AvocaStructure.PaymentFrequency = 6;

            //Set up the tranches
            TempDate = new DateTime(2014, 3, 11);
            ManTranche TempTranche = new ManTranche();
            TempTranche.TrancheName = "AVOCA VI-X A1";
            TempTranche.PaymentFrequency = 6;
            TempTranche.SettlementDate = TempDate;
            TempTranche.Currency = "EUR";
            TempTranche.ProrataGroup = 1;
            TempTranche.OriginalAmount = 301500000;
            TempTranche.OutstandingAmt = 255379093;
            TempTranche.InterestType = ManTranche.ManTrancheInterestType.FloatingInterest;
            TempTranche.Price = 100.0;
            TempTranche.Coupon = "0.0021";
            TempTranche.ReferenceRate = "EUR006M";
            TempTranche.DefaultRefRate = "EUR006M";
            TempTranche.ReferenceRateValue = 0.0038;
            TempTranche.AccruedInterest = 0.09/100.0;
            TempDate = new DateTime(2014, 1, 16);
            TempTranche.LastPaymentDate = TempDate;
            AvocaStructure.AddTranche(TempTranche);
            TempTranche.TrancheName = "AVOCA VI-X A2";
            TempTranche.ProrataGroup = 2;
            TempTranche.MinOClevel = 1.258;
            TempTranche.OriginalAmount = 64000000;
            TempTranche.OutstandingAmt = 64000000;
            TempTranche.Coupon = "0.0028";
            TempTranche.AccruedInterest = 0.1 / 100.0;
            AvocaStructure.AddTranche(TempTranche);
            TempTranche.TrancheName = "AVOCA VI-X B";
            TempTranche.ProrataGroup = 3;
            TempTranche.MinOClevel = 1.209;
            TempTranche.MinIClevel = 1.2;
            TempTranche.OriginalAmount = 19400000;
            TempTranche.OutstandingAmt = 19400000;
            TempTranche.Coupon = "0.0035";
            TempTranche.AccruedInterest = 0.11 / 100.0;
            AvocaStructure.AddTranche(TempTranche);
            TempTranche.TrancheName = "AVOCA VI-X C";
            TempTranche.ProrataGroup = 4;
            TempTranche.MinOClevel = 1.141;
            TempTranche.MinIClevel = 1.1;
            TempTranche.OriginalAmount = 31500000;
            TempTranche.OutstandingAmt = 31500000;
            TempTranche.Coupon = "0.0055";
            TempTranche.AccruedInterest = 0.14 / 100.0;
            AvocaStructure.AddTranche(TempTranche);
            TempTranche.TrancheName = "AVOCA VI-X D";
            TempTranche.ProrataGroup = 5;
            TempTranche.MinOClevel = 1.096;
            TempTranche.MinIClevel = 1.05;
            TempTranche.OriginalAmount = 20000000;
            TempTranche.OutstandingAmt = 20000000;
            TempTranche.Coupon = "0.0135";
            TempTranche.AccruedInterest = 0.27 / 100.0;
            AvocaStructure.AddTranche(TempTranche);
            TempTranche.TrancheName = "AVOCA VI-X E";
            TempTranche.ProrataGroup = 6;
            TempTranche.MinOClevel = 1.046;
            TempTranche.MinIClevel = 1.025;
            TempTranche.OriginalAmount = 23850000;
            TempTranche.OutstandingAmt = 23850000;
            TempTranche.Coupon = "0.0335";
            TempTranche.AccruedInterest = 0.57 / 100.0;
            AvocaStructure.AddTranche(TempTranche);
            TempTranche.TrancheName = "AVOCA VI-X F";
            TempTranche.ProrataGroup = 7;
            TempTranche.MinOClevel = 1.03;
            TempTranche.MinIClevel = 1.0;
            TempTranche.OriginalAmount = 10000000;
            TempTranche.OutstandingAmt = 10000000;
            TempTranche.Coupon = "0.0495";
            TempTranche.AccruedInterest = 0.82 / 100.0;
            AvocaStructure.AddTranche(TempTranche);
            TempTranche.TrancheName = "AVOCA VI-X M";
            TempTranche.ProrataGroup = 8;
            TempTranche.MinOClevel = 0;
            TempTranche.MinIClevel = 0;
            TempTranche.OriginalAmount = 37750000;
            TempTranche.OutstandingAmt = 37750000;
            TempTranche.InterestType = ManTranche.ManTrancheInterestType.FixedInterest;
            TempTranche.Coupon = "0";
            TempTranche.AccruedInterest = 0.0;
            AvocaStructure.AddTranche(TempTranche);

            //Set up the waterfall
            ManWatFalPrior TempStep = new ManWatFalPrior();
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_SeniorFees;
            TempStep.RedemptionGroup = (int)ManWatFalPrior.SourceOfFunding.sof_Interest;
            AvocaStructure.AddStep(TempStep);
            TempStep.RedemptionGroup = (int)ManWatFalPrior.SourceOfFunding.sof_Principal;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_SeniorExpenses;
            TempStep.RedemptionGroup = (int)ManWatFalPrior.SourceOfFunding.sof_Interest;
            AvocaStructure.AddStep(TempStep);
            TempStep.RedemptionGroup = (int)ManWatFalPrior.SourceOfFunding.sof_Principal;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_Interest;
            TempStep.GroupTarget = 1;
            AvocaStructure.AddStep(TempStep);
            TempStep.GroupTarget = 2;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_OCTest;
            TempStep.RedemptionGroup = 0;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_Interest;
            TempStep.GroupTarget = 3;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_DeferredInterest;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_OCTest;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_ICTest;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_Interest;
            TempStep.GroupTarget = 4;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_DeferredInterest;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_OCTest;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_ICTest;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_Interest;
            TempStep.GroupTarget = 5;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_DeferredInterest;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_OCTest;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_ICTest;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_Interest;
            TempStep.GroupTarget = 6;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_DeferredInterest;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_OCTest;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_ICTest;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_Interest;
            TempStep.GroupTarget = 7;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_DeferredInterest;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_OCTest;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_ICTest;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_ReinvestmentTest;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_juniorFees;
            TempStep.RedemptionGroup = (int)ManWatFalPrior.SourceOfFunding.sof_Interest;
            AvocaStructure.AddStep(TempStep);
            TempStep.GroupTarget = 1;
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_DeferredPrinc;
            AvocaStructure.AddStep(TempStep);
            TempStep.GroupTarget = 2;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_OCTestPrinc;
            TempStep.RedemptionGroup = 0;
            AvocaStructure.AddStep(TempStep);
            TempStep.GroupTarget = 3;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_ICTestPrinc;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_OCTestPrinc;
            TempStep.GroupTarget = 4;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_ICTestPrinc;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_OCTestPrinc;
            TempStep.GroupTarget = 5;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_ICTestPrinc;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_OCTestPrinc;
            TempStep.GroupTarget = 6;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_ICTestPrinc;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_OCTestPrinc;
            TempStep.GroupTarget = 7;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_ICTestPrinc;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_ReinvestPrincipal;
            AvocaStructure.AddStep(TempStep);
            TempStep.GroupTarget = 1;
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_Principal;
            AvocaStructure.AddStep(TempStep);
            TempStep.GroupTarget = 2;
            AvocaStructure.AddStep(TempStep);
            TempStep.GroupTarget = 3;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_DeferredPrinc;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_Principal;
            TempStep.GroupTarget = 4;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_DeferredPrinc;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_Principal;
            TempStep.GroupTarget = 5;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_DeferredPrinc;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_Principal;
            TempStep.GroupTarget = 6;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_DeferredPrinc;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_Principal;
            TempStep.GroupTarget = 7;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_DeferredPrinc;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_juniorFees;
            TempStep.RedemptionGroup = (int)ManWatFalPrior.SourceOfFunding.sof_Principal;
            AvocaStructure.AddStep(TempStep);
            TempStep.PriorityType = ManWatFalPrior.ManWaterfallStepType.wst_Excess;
            TempStep.RedemptionGroup = 8;
            AvocaStructure.AddStep(TempStep);

            ManCentralUnit ModelUnit=new ManCentralUnit();
            ModelUnit.Structure=AvocaStructure;
            ModelUnit.RunCall=true;
            TempDate = new DateTime(2014, 1, 31);
            ModelUnit.CutOffDate=TempDate;
            StreamReader LoansReader = File.OpenText(TestCommons.WorkDir + "AVOCA Loan Pool.csv");
            ManMortgage TempMtg = new ManMortgage();
            while (!LoansReader.EndOfStream)
            {
                String LoanString = LoansReader.ReadLine();
                String[] SingleParts = LoanString.Split(new Char[] { ',' });
                Assert.AreEqual<int>(SingleParts.Length, 7, "Loan pool doesn't have 7 fields");
                if (double.Parse(SingleParts[6]) > 0){
                    TempMtg.Interest = SingleParts[0];
                    TempMtg.Annuity = SingleParts[1];
                    TempMtg.PaymentFreq = int.Parse(SingleParts[2]);
                    TempMtg.PrepayMultiplier = SingleParts[3];
                    TempMtg.LossMultiplier = SingleParts[4];
                    TempMtg.MaturityDate = DateTime.Parse(SingleParts[5]);
                    TempMtg.Size = double.Parse(SingleParts[6]);
                    ModelUnit.AddLoan(TempMtg);
                }
            }
            LoansReader.Close();
            var finished = new ManualResetEvent(false);
            ModelUnit.CalculationFinishedEvent += delegate
            {
                finished.Set();
            };
            ModelUnit.Calculate();
            Assert.IsTrue(finished.WaitOne(1000), "Calculation not finished");
            ModelUnit.SaveBaseCase(TestCommons.WorkDir, "AVOCA Base Case");
            FileStream TmpFstream=File.OpenWrite(TestCommons.WorkDir + "AVOCA Managed File.mclo");
            ModelUnit.Result.Write(TmpFstream);
            TmpFstream.Close();

            ManWaterfall TestRead=new ManWaterfall();
            FileStream TmpFstreamIn = File.OpenRead(TestCommons.WorkDir + "AVOCA Managed File.mclo");
            TestRead.Read(TmpFstreamIn);
            TmpFstreamIn.Close();
        }
    }
}
