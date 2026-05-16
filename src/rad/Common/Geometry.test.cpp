#include <rad/Common/Geometry.h>

#include <gtest/gtest.h>

using namespace rad;

TEST(Geometry, Basics)
{
    {
        EXPECT_TRUE(AlmostEqual(Normalize(Vec2f{}), Vec2f{}));
        const Vec2f diagonal{3.f, 4.f};
        EXPECT_NEAR(Length(Normalize(diagonal)), 1.f, 1e-5f);
    }

    {
        EXPECT_NEAR(AngleBetween(Vec3f{1.f, 0.f, 0.f}, Vec3f{0.f, 1.f, 0.f}),
                    static_cast<float>(M_PI_2), 1e-5f);
        EXPECT_NEAR(AngleBetween(Vec3f{1.f, 0.f, 0.f}, Vec3f{-1.f, 0.f, 0.f}),
                    static_cast<float>(M_PI), 1e-5f);
        EXPECT_FLOAT_EQ(AngleBetween(Vec2f{}, Vec2f{1.f, 0.f}), 0.f);
        EXPECT_NEAR(AngleBetween(Vec2f{1.f, 0.f}, Vec2f{0.f, 1.f}), static_cast<float>(M_PI_2),
                    1e-5f);
    }

    {
        const Vec2f v{1.f, 2.f};
        EXPECT_TRUE(AlmostEqual(Rotate90CCW(Rotate90CW(v)), v));
        EXPECT_TRUE(AlmostEqual(Rotate90CW(Vec2f{1.f, 0.f}), Vec2f{0.f, -1.f}));
    }

    {
        const OrthoFrame2f identity{};
        const OrthoFrame2f fromZeroAxis = MakeOrthoFrame(Vec2f{});
        EXPECT_TRUE(AlmostEqual(fromZeroAxis.m_x, identity.m_x));
        EXPECT_TRUE(AlmostEqual(fromZeroAxis.m_y, identity.m_y));

        const OrthoFrame2f frame = MakeOrthoFrame(Vec2f{2.f, 0.f});
        EXPECT_NEAR(Length(frame.m_x), 1.f, 1e-5f);
        EXPECT_NEAR(Cross(frame.m_x, frame.m_y), 1.f, 1e-5f);
        const Vec2f world{3.f, 4.f};
        EXPECT_TRUE(AlmostEqual(frame.FromLocal(frame.ToLocal(world)), world, 1e-5f));
    }

    {
        const OrthoFrame3f identity{};
        const OrthoFrame3f fromZeroAxis = MakeOrthoFrame(Vec3f{});
        EXPECT_TRUE(AlmostEqual(fromZeroAxis.m_x, identity.m_x));
        EXPECT_TRUE(AlmostEqual(fromZeroAxis.m_y, identity.m_y));
        EXPECT_TRUE(AlmostEqual(fromZeroAxis.m_z, identity.m_z));

        const OrthoFrame3f frame = MakeOrthoFrame(Vec3f{0.f, 0.f, 2.f});
        EXPECT_NEAR(Length(frame.m_z), 1.f, 1e-5f);
        EXPECT_NEAR(Dot(frame.m_x, frame.m_y), 0.f, 1e-6f);
        EXPECT_TRUE(AlmostEqual(Cross(frame.m_x, frame.m_y), frame.m_z, 1e-5f));
        const Vec3f world{1.f, 2.f, 3.f};
        EXPECT_TRUE(AlmostEqual(frame.FromLocal(frame.ToLocal(world)), world, 1e-5f));

        const OrthoFrame3f down = MakeOrthoFrame(Vec3f{0.f, 0.f, -3.f});
        EXPECT_TRUE(AlmostEqual(Cross(down.m_x, down.m_y), down.m_z, 1e-5f));
    }

    {
        const Normal3f up{0.f, 1.f, 0.f};
        EXPECT_TRUE(AlmostEqual(Reflect(Vec3f{1.f, -1.f, 0.f}, up), Vec3f{1.f, 1.f, 0.f}));
        EXPECT_TRUE(AlmostEqual(FaceForward(up, Vec3f{0.f, -1.f, 0.f}), Normal3f{0.f, -1.f, 0.f}));
        EXPECT_TRUE(AlmostEqual(FaceForward(up, Vec3f{0.f, 1.f, 0.f}), up));

        const Normal2f right{1.f, 0.f};
        EXPECT_TRUE(AlmostEqual(Reflect(Vec2f{-1.f, 1.f}, right), Vec2f{1.f, 1.f}));
        EXPECT_TRUE(AlmostEqual(FaceForward(right, Vec2f{-1.f, 0.f}), Normal2f{-1.f, 0.f}));
    }
}

TEST(Geometry, BoundsTest)
{
    const Bounds2f rect = Bounds2f::FromPoints({0.f, 0.f}, {2.f, 4.f});
    const Bounds2f overlapping = Bounds2f::FromPoints({1.f, 1.f}, {3.f, 3.f});
    const Bounds2f separated = Bounds2f::FromPoints({5.f, 0.f}, {6.f, 1.f});
    const Bounds2f edgeTouching = Bounds2f::FromPoints({2.f, 0.f}, {3.f, 1.f});

    EXPECT_TRUE(rect.Intersects(overlapping));
    EXPECT_FALSE(rect.Intersects(separated));
    EXPECT_TRUE(rect.Intersects(edgeTouching));
    EXPECT_FLOAT_EQ(Intersect(rect, overlapping).Area(), 2.f);
    EXPECT_TRUE(Intersect(rect, separated).IsEmpty());
    EXPECT_TRUE(Intersect(rect, edgeTouching).IsEmpty());

    Bounds2f expanded;
    EXPECT_TRUE(expanded.IsEmpty());
    expanded.Expand({0.f, 0.f});
    expanded.Expand({1.f, 1.f});
    EXPECT_FALSE(expanded.IsEmpty());
    EXPECT_TRUE(AlmostEqual(expanded.Center(), Point2f{0.5f, 0.5f}));

    const Bounds2f boundsFromCenter = Bounds2f::FromCenterHalfSize({1.f, 2.f}, {1.f, 2.f});
    EXPECT_TRUE(AlmostEqual(boundsFromCenter.m_min, rect.m_min));
    EXPECT_TRUE(AlmostEqual(boundsFromCenter.m_max, rect.m_max));

    EXPECT_TRUE(Inside(Point2f{1.f, 1.f}, rect));
    EXPECT_TRUE(rect.Contains(Point2f{2.f, 4.f}));
    EXPECT_TRUE(InsideExclusive(Point2f{0.f, 0.f}, rect));
    EXPECT_FALSE(InsideExclusive(Point2f{2.f, 4.f}, rect));

    EXPECT_FLOAT_EQ(DistanceSquared(Point2f{3.f, 1.f}, rect), 1.f);
    EXPECT_FLOAT_EQ(DistanceSquared(Point2f{1.f, 1.f}, rect), 0.f);
    EXPECT_TRUE(AlmostEqual(ClosestPoint(Point2f{3.f, 1.f}, rect), Point2f{2.f, 1.f}));

    const Bounds2f merged = rect.Union(separated);
    EXPECT_TRUE(merged.Contains(Point2f{5.5f, 0.5f}));
    EXPECT_TRUE(AlmostEqual(rect.Lerp(Vec2f{0.5f, 0.25f}), Point2f{1.f, 1.f}));
    EXPECT_TRUE(AlmostEqual(rect.NormalizedOffset(Point2f{1.f, 2.f}), Vec2f{0.5f, 0.5f}));

    EXPECT_TRUE(AlmostEqual(rect.GetCorner(0), Point2f{0.f, 0.f}));
    EXPECT_TRUE(AlmostEqual(rect.GetCorner(3), Point2f{2.f, 4.f}));
    EXPECT_EQ(rect.MaxDimensionIndex(), 1);

    {
        Bounds2f empty;
        const Circlef unitCircle{{0.f, 0.f}, 1.f};
        EXPECT_TRUE(empty.IsEmpty());
        EXPECT_TRUE(std::isinf(DistanceSquared(Point2f{0.f, 0.f}, empty)));
        EXPECT_FALSE(unitCircle.Intersects(empty));
    }

    const Bounds3f cube = Bounds3f::FromPoints({0.f, 0.f, 0.f}, {2.f, 2.f, 2.f});
    EXPECT_FLOAT_EQ(cube.Volume(), 8.f);
    EXPECT_FLOAT_EQ(cube.SurfaceArea(), 24.f);
    EXPECT_TRUE(Inside(Point3f{1.f, 1.f, 1.f}, cube));
    EXPECT_FLOAT_EQ(DistanceSquared(Point3f{3.f, 1.f, 1.f}, cube), 1.f);
    EXPECT_FLOAT_EQ(Distance(Point3f{3.f, 1.f, 1.f}, cube), 1.f);
    EXPECT_TRUE(AlmostEqual(ClosestPoint(Point3f{3.f, 1.f, 1.f}, cube), Point3f{2.f, 1.f, 1.f}));

    {
        Bounds3f empty;
        const Spheref unitSphere{{0.f, 0.f, 0.f}, 1.f};
        EXPECT_TRUE(std::isinf(DistanceSquared(Point3f{1.f, 1.f, 1.f}, empty)));
        EXPECT_FALSE(unitSphere.Intersects(empty));
    }
}

TEST(Geometry, BoundsIntegerTest)
{
    const Bounds2i pixelRect = Bounds2i::FromPoints({0, 0}, {4, 2});
    EXPECT_EQ(pixelRect.Width(), 4);
    EXPECT_EQ(pixelRect.Height(), 2);
    EXPECT_EQ(pixelRect.Area(), 8);
    EXPECT_TRUE(pixelRect.Contains(Point2<int>{2, 1}));
    EXPECT_FALSE(pixelRect.Intersects(Bounds2i::FromPoints({5, 0}, {6, 1})));
}

TEST(Geometry, CircleTest)
{
    const Circlef unit{{0.f, 0.f}, 1.f};
    const Circlef radiusTwo{{0.f, 0.f}, 2.f};
    const Circlef pointRadius{{1.f, 0.f}, 0.f};

    EXPECT_NEAR(unit.Area(), static_cast<float>(M_PI), 1e-5f);
    EXPECT_NEAR(unit.Perimeter(), static_cast<float>(2 * M_PI), 1e-5f);
    EXPECT_NEAR(radiusTwo.Area(), static_cast<float>(4 * M_PI), 1e-4f);

    EXPECT_TRUE(unit.Contains(unit.m_center));
    EXPECT_TRUE(unit.Contains(Point2f{0.5f, 0.f}));
    EXPECT_TRUE(unit.Contains(Point2f{1.f, 0.f}));
    EXPECT_FALSE(unit.Contains(Point2f{1.0001f, 0.f}));

    EXPECT_TRUE(pointRadius.Contains(Point2f{1.f, 0.f}));
    EXPECT_FALSE(pointRadius.Contains(Point2f{1.f, 0.0001f}));
    EXPECT_TRUE(unit.Intersects(pointRadius));

    const Circlef partiallyOverlapping{{1.5f, 0.f}, 1.f};
    const Circlef separated{{3.f, 0.f}, 1.f};
    const Circlef externallyTangent{{2.f, 0.f}, 1.f};
    const Circlef concentricInner{{0.f, 0.f}, 0.5f};
    EXPECT_TRUE(unit.Intersects(partiallyOverlapping));
    EXPECT_FALSE(unit.Intersects(separated));
    EXPECT_TRUE(unit.Intersects(externallyTangent));
    EXPECT_TRUE(radiusTwo.Intersects(concentricInner));

    const Bounds2f unitSquare = Bounds2f::FromPoints({-1.f, -1.f}, {1.f, 1.f});
    const Bounds2f eastSlab = Bounds2f::FromPoints({2.f, -1.f}, {3.f, 1.f});
    const Circlef distant{{10.f, 0.f}, 0.5f};
    const Circlef reachingSlab{{1.5f, 0.f}, 1.f};
    const Circlef tangentToSquare{{2.f, 0.f}, 1.f};
    const Circlef enclosing{{0.f, 0.f}, 3.f};
    EXPECT_TRUE(unit.Intersects(unitSquare));
    EXPECT_TRUE(Intersects(unit, unitSquare));
    EXPECT_EQ(unit.Intersects(unitSquare), Intersects(unit, unitSquare));
    EXPECT_FALSE(Intersects(distant, unitSquare));
    EXPECT_TRUE(Intersects(reachingSlab, eastSlab));
    EXPECT_TRUE(tangentToSquare.Intersects(unitSquare));
    EXPECT_TRUE(enclosing.Intersects(unitSquare));
    EXPECT_TRUE(unit.Contains(Point2f{0.f, 0.f}));
}

TEST(Geometry, SphereTest)
{
    const Spheref unit{{0.f, 0.f, 0.f}, 1.f};
    const Spheref radiusTwo{{0.f, 0.f, 0.f}, 2.f};
    const Spheref pointRadius{{0.f, 1.f, 0.f}, 0.f};

    EXPECT_NEAR(unit.Volume(), static_cast<float>(4. / 3. * M_PI), 1e-5f);
    EXPECT_NEAR(unit.SurfaceArea(), static_cast<float>(4 * M_PI), 1e-5f);
    EXPECT_NEAR(radiusTwo.Volume(), static_cast<float>(32. / 3. * M_PI), 1e-4f);

    EXPECT_TRUE(unit.Contains(unit.m_center));
    EXPECT_TRUE(unit.Contains(Point3f{0.f, 0.f, 1.f}));
    EXPECT_FALSE(unit.Contains(Point3f{0.f, 0.f, 1.0001f}));

    EXPECT_TRUE(pointRadius.Contains(Point3f{0.f, 1.f, 0.f}));
    EXPECT_FALSE(pointRadius.Contains(Point3f{0.0001f, 1.f, 0.f}));
    EXPECT_TRUE(unit.Intersects(pointRadius));

    const Spheref partiallyOverlapping{{1.5f, 0.f, 0.f}, 1.f};
    const Spheref separated{{3.f, 0.f, 0.f}, 1.f};
    const Spheref externallyTangent{{2.f, 0.f, 0.f}, 1.f};
    const Spheref concentricInner{{0.f, 0.f, 0.f}, 0.5f};
    EXPECT_TRUE(unit.Intersects(partiallyOverlapping));
    EXPECT_FALSE(unit.Intersects(separated));
    EXPECT_TRUE(unit.Intersects(externallyTangent));
    EXPECT_TRUE(radiusTwo.Intersects(concentricInner));

    const Bounds3f unitCube = Bounds3f::FromPoints({-1.f, -1.f, -1.f}, {1.f, 1.f, 1.f});
    const Bounds3f eastSlab = Bounds3f::FromPoints({2.f, -1.f, -1.f}, {3.f, 1.f, 1.f});
    const Spheref distant{{5.f, 0.f, 0.f}, 0.5f};
    const Spheref reachingSlab{{1.5f, 0.f, 0.f}, 1.f};
    EXPECT_TRUE(unit.Intersects(unitCube));
    EXPECT_TRUE(Intersects(unit, unitCube));
    EXPECT_EQ(unit.Intersects(unitCube), Intersects(unit, unitCube));
    EXPECT_FALSE(distant.Intersects(unitCube));
    EXPECT_FALSE(Intersects(distant, unitCube));
    EXPECT_TRUE(reachingSlab.Intersects(eastSlab));
    EXPECT_TRUE(Intersects(reachingSlab, eastSlab));
}

TEST(Geometry, LineSegmentTest)
{
    const LineSegment2f segment34{Point2f{0.f, 0.f}, Point2f{3.f, 4.f}};
    EXPECT_FLOAT_EQ(segment34.LengthSquared(), 25.f);
    EXPECT_NEAR(segment34.Length(), 5.f, 1e-5f);
    EXPECT_NEAR(segment34.Length(), std::sqrt(segment34.LengthSquared()), 1e-6f);
    EXPECT_TRUE(AlmostEqual(segment34.Center(), Point2f{1.5f, 2.f}));
    EXPECT_TRUE(AlmostEqual(segment34.Direction(), Vec2f{0.6f, 0.8f}, 1e-5f));

    const LineSegment2f reversed{Point2f{3.f, 4.f}, Point2f{0.f, 0.f}};
    EXPECT_FLOAT_EQ(reversed.LengthSquared(), segment34.LengthSquared());
    EXPECT_TRUE(AlmostEqual(reversed.Direction(), Vec2f{-0.6f, -0.8f}, 1e-5f));

    const LineSegment2f zeroLength{Point2f{1.f, 2.f}, Point2f{1.f, 2.f}};
    EXPECT_FLOAT_EQ(zeroLength.LengthSquared(), 0.f);
    EXPECT_FLOAT_EQ(zeroLength.Length(), 0.f);
    EXPECT_TRUE(AlmostEqual(zeroLength.Center(), Point2f{1.f, 2.f}));
    EXPECT_TRUE(AlmostEqual(zeroLength.Direction(), Vec2f{}));

    const LineSegment3f segment{Point3f{0.f, 0.f, 0.f}, Point3f{1.f, 2.f, 2.f}};
    EXPECT_FLOAT_EQ(segment.LengthSquared(), 9.f);
    EXPECT_NEAR(segment.Length(), 3.f, 1e-5f);
    EXPECT_TRUE(AlmostEqual(segment.Center(), Point3f{0.5f, 1.f, 1.f}));
    EXPECT_TRUE(AlmostEqual(segment.Direction(), Vec3f{1.f / 3.f, 2.f / 3.f, 2.f / 3.f}, 1e-5f));

    const LineSegment3f defaultConstructed{};
    EXPECT_FLOAT_EQ(defaultConstructed.LengthSquared(), 0.f);
    EXPECT_TRUE(AlmostEqual(defaultConstructed.Direction(), Vec3f{}));
}
