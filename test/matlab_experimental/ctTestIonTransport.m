classdef ctTestIonTransport < ctTestCase

    properties
        phase
    end

    properties (SetAccess = protected)
        rtol = 1e-6;
        atol = 1e-8;
    end

    methods (TestMethodSetup)

        function createPhase(self)
            src = '../data/ch4_ion.yaml';
            self.phase = Solution(src);
            self.phase.TPX = {2237, OneAtm, 'O2:0.7010, H2O:0.1885, CO2:9.558e-2'};
        end

    end

    methods (Test)

        function testBinaryDiffusion(self)
            N2_idx = self.phase.speciesIndex('N2');
            H3Op_idx = self.phase.speciesIndex('H3O+');
            bdiff = self.phase.binDiffCoeffs(N2_idx, H3Op_idx);

            self.verifyEqual(bdiff, 4.258e-4, 'RelTol', 1e-4);
        end

        function testMixtureDiffusion(self)
            H3Op_idx = self.phase.speciesIndex('H3O+');
            O2m_idx = self.phase.speciesIndex('O2-');
            mdiff1 = self.phase.mixDiffCoeffs(H3Op_idx);
            mdiff2 = self.phase.mixDiffCoeffs(O2m_idx);

            self.verifyEqual(mdiff1, 5.057e-4, 'RelTol', 1e-4);
            self.verifyEqual(mdiff2, 2.784e-4, 'RelTol', 1e-3);
        end

        function testUpdateTemperature(self)
            N2_idx = self.phase.speciesIndex('N2');
            H3Op_idx = self.phase.speciesIndex('H3O+');
            bdiff1 = self.phase.binDiffCoeffs(N2_idx, H3Op_idx);
            mdiff1 = self.phase.mixDiffCoeffs(H3Op_idx);

            self.phase.TP = {0.9 * self.phase.T, self.phase.P};
            bdiff2 = self.phase.binDiffCoeffs(N2_idx, H3Op_idx);
            mdiff2 = self.phase.mixDiffCoeffs(H3Op_idx);

            self.verifyNotEqual(bdiff1, bdiff2);
            self.verifyNotEqual(mdiff1, mdiff2);
        end


        function testIonizedGasWithoutIons(self)
            gas = Solution('h2o2.yaml');
            gas.TPX = {800, 2*OneAtm, ...
                       [0.1, 1e-4, 1e-5, 0.2, 2e-4, 0.3, 1e-6, 5e-5, 1e-6, 0.4]};

            gas.transportModel = 'ionized-gas';
            Dkm1 = gas.mixDiffCoeffs;
            Dbin1 = gas.binDiffCoeffs;

            gas.transportModel = 'mixture-averaged';
            Dkm2 = gas.mixDiffCoeffs;
            Dbin2 = gas.binDiffCoeffs;

            self.verifyEqual(Dkm1, Dkm2, 'AbsTol', self.atol);
            self.verifyEqual(Dbin1, Dbin2, 'AbsTol', self.atol);
        end

        function testIonizedLowT(self)
            gas = Solution('../data/ET_test.yaml');

            kO2m = gas.speciesIndex('O2^-');
            kC10H8 = gas.speciesIndex('C10H8');

            gas.TP = {300, OneAtm};
            Dbin = gas.binDiffCoeffs;
            self.verifyEqual(Dbin(kO2m, kC10H8), 2.18902175e-06, ...
                             'RelTol', self.rtol);

            gas.TP = {350, OneAtm};
            Dbin = gas.binDiffCoeffs;
            self.verifyEqual(Dbin(kO2m, kC10H8), 2.92899733e-06, ...
                             'RelTol', self.rtol);
        end

    end

end
